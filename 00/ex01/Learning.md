# ex01 学習記録

CPP Module 00 ex01（PhoneBook）の実装・設計を理解する過程で整理したメモ。

---

## 1. Contact クラスとカプセル化

### private / public

```cpp
class Contact {
private:
    std::string _firstName;
    // ...
public:
    std::string getFirstName(void) const;
};
```

| 指定 | 意味 |
|---|---|
| `private` | クラス外から直接触れない（内部データ） |
| `public` | 外から使える API（コンストラクタ、getter など） |

```cpp
Contact c("Alice", "Smith", ...);
c._firstName = "Bob";  // NG: private
c.getFirstName();      // OK: public
```

**カプセル化** = 内部表現を隠し、public メソッド経由だけでアクセスする。

### setter がない設計（E案 / 値オブジェクト）

- **作る:** コンストラクタ
- **読む:** getter / `printDetails`
- **変える:** 不可（新しい `Contact` を作って差し替え）

1人分のデータは値オブジェクトとして扱い、`PhoneBook` がコレクションとして寿命と更新を管理する。

---

## 2. メンバ名の `_` プレフィックス

`_firstName` のように `_` を付けるのは **「これはメンバ変数」** と区別する慣習（42 でもよく使う）。

| 観点 | 説明 |
|---|---|
| C++98 標準の推奨？ | **No**（命名規則は標準にない） |
| 42 subject の必須？ | **No** |
| 42 サンプル（`Account.hpp`） | **Yes**（de facto の慣習） |
| Python 的な `_` | private シグナル（慣習のみ） |
| C++ での主目的 | **引数・ローカル変数との名前の区別** |

`private:` があるので「プライベートであること」を `_` で再度示すのは冗長な面もある。ex01 では Account 系サンプルに合わせるなら `_` 始まり、個人プロジェクトなら `_` なし + 初期化リストや `this->` でもよい。

---

## 3. コンストラクタ（2種類）

### ① デフォルトコンストラクタ `Contact(void)`

```cpp
Contact::Contact(void) {}
```

- 引数なしで `Contact` を作る
- 本体が空 `{}` でも、各 `std::string` メンバはデフォルトコンストラクタで **空文字** になる

### ② 5引数コンストラクタ（ADD 用）

```cpp
Contact::Contact(const std::string& firstName, ...)
    : _firstName(firstName),
      _lastName(lastName),
      ...
{}
```

- **初期化リスト** `: _firstName(firstName), ...` が推奨（メンバ生成時に値をセット）
- `const std::string&` = const 参照（コピーを避け、関数内で変更しない）

### どちらが呼ばれるか

**呼び出し時の引数の数（と型）** でコンパイラが決める（オーバーロード）。

```cpp
Contact c1;                              // → Contact(void)
Contact c2("A", "B", "C", "D", "E");    // → 5引数版

return Contact(firstName, ...);          // → 5引数版（promptContact）
```

### コンストラクタが走るタイミング

| 段階 | コンストラクタ |
|---|---|
| `class Contact { ... };` を書いた | **呼ばれない**（型定義だけ） |
| `Contact c;` | `Contact(void)` |
| `Contact c(...);` | 5引数版 |
| `PhoneBook phoneBook;` | `PhoneBook::PhoneBook()` + `Contact(void)` × 8 |

---

## 4. なぜデフォルトコンストラクタが必要か

```cpp
class PhoneBook {
private:
    Contact _contacts[8];
};
```

`PhoneBook phoneBook;` と書いた **その瞬間** に:

1. `PhoneBook` 用のメモリ確保
2. `_contacts[0]` .. `_contacts[7]` それぞれに `Contact(void)` が呼ばれる
3. `_contactCount`, `_oldestIndex` を初期化
4. `PhoneBook` コンストラクタ本体 `{}`

**ADD するまで Contact が存在しないわけではない。** 起動時点で8スロット分の空 `Contact` が確保される。

### 「5引数版で空文字を渡せばいいのでは？」

中身としては同等だが、**配列メンバの都合でデフォルトコンストラクタは依然必要**。

- 5引数版だけにすると `Contact _contacts[8];` は **コンパイルエラー**
- `PhoneBook` コンストラクタ内で `Contact("", "", "", "", "")` を代入しても、その前に `_contacts[8]` はすでに `Contact(void)` で構築されている

### ADD 時の流れ

```
promptContact()
  └─ return Contact(...);     // 5引数版で一時オブジェクト作成
addContact(その Contact)
  └─ _contacts[i] = contact;  // 代入（コンストラクタではない）
```

---

## 5. getter と printDetails

### getter

```cpp
std::string Contact::getFirstName(void) const {
    return this->_firstName;
}
```

| キーワード | 意味 |
|---|---|
| 末尾の `const` | このメソッドはオブジェクトを変更しない |
| `this` | 今のオブジェクト自身 |
| 戻り値 `std::string` | コピーを返す（外側から元データを書き換えられない） |

SEARCH の一覧表示では `PhoneBook` が getter で値を読む。

### printDetails

詳細表示の形式を `Contact` 側に持たせる設計（D案）。

- **一覧:** `PhoneBook` が getter で列を組み立て
- **詳細:** `Contact` が自分で5行表示

---

## 6. スタック vs ヒープ（C++ vs Python）

### C++：`PhoneBook phoneBook;`

```cpp
int main(void) {
    PhoneBook phoneBook;  // スタック上に PhoneBook 本体（Contact 8個込み）
}
```

- 変数 **自体が** オブジェクト本体（ポインタではない）
- スコープを抜けると自動破棄（デストラクタ）
- `new` / `delete` 不要

```
スタック上の phoneBook
├─ _contacts[0] .. _contacts[7]
├─ _contactCount
└─ _oldestIndex
```

### C++：`new` を使う場合

```cpp
PhoneBook* phoneBook = new PhoneBook();
delete phoneBook;
```

- スタックには **ポインタだけ**
- 本体はヒープ
- `delete` 忘れでリーク

### Python

```python
phone_book = PhoneBook()
```

- 変数は **参照（名前）**
- インスタンスは **ほぼ常にヒープ**
- GC が回収

### 言語ごとの違い

| 言語 | オブジェクトの置き場所 | 変数が持つもの |
|---|---|---|
| C++ | スタック or ヒープ（選べる） | オブジェクト本体 or ポインタ |
| Python / Java（class） | ほぼ常にヒープ | 参照 |

### なぜ C++ はスタックに置けるか

| 理由 | 説明 |
|---|---|
| 歴史 | C からの延長（ローカル変数はスタックが基本） |
| 性能 | スタック確保・解放が速い |
| RAII | スコープ終了で確実にデストラクタ |
| GC 不要 | いつ破棄されるかコードで決められる |

Python 式（全部ヒープ + GC）の方が **書く側はシンプル**。C++ は **制御と性能** のトレードオフ。

### ex01 でスタックを使う理由

- 件数固定（8件）
- 寿命が `main` と一致
- 動的確保（`new`）禁止

→ `PhoneBook phoneBook;` が最も自然。

### `std::string` の注意

`Contact` 本体はスタック上でも、`std::string` が **文字データを内部でヒープに持つ** ことはある（実装依存）。

---

## 7. クラス定義 vs インスタンス

### クラス定義（`.hpp`）の時点

```cpp
class Contact {
    std::string _firstName;
};
```

- **型の設計図**。インスタンス用メモリはまだ取らない
- コンストラクタは **呼ばれない**

### インスタンス生成の時点

```cpp
Contact c;              // ここで初めてメモリ確保 + Contact(void)
PhoneBook phoneBook;    // PhoneBook + Contact × 8
```

### Python との対比

| | Python | C++ |
|---|---|---|
| `class Contact:` | クラスオブジェクトがヒープに作られる | 型としてコンパイラが処理（クラスオブジェクトは作られない） |
| `Contact()` | インスタンスがヒープに作られる | インスタンスがスタック/ヒープに作られる |

C++ では **「型はコンパイル時、実体は変数宣言時」**。

---

## 8. メソッドテーブル（vtable）

ex01 の `Contact` / `PhoneBook` は **`virtual` 関数なし**。

- オブジェクトごとのメソッドテーブルは **基本ない**
- `getFirstName()` の呼び出し先は **コンパイル時に決定**
- オブジェクトの中身は **データメンバだけ**

```
Contact オブジェクト
├─ _firstName
├─ _lastName
└─ ...
（メソッド情報はオブジェクト内に載らない）
```

`virtual` があるクラス（Module 02 以降）では、オブジェクトに **vtable へのポインタ** が載ることがある。

---

## 9. コンパイル時 vs 実行時

### C++（コンパイラ言語）

```
Contact.hpp + Contact.cpp
    ↓ コンパイル
Contact.o（機械語）
    ↓ リンク
phonebook（実行ファイル）
    ↓ 実行
PhoneBook phoneBook;  ← ここで初めてインスタンスデータ確保
```

- 型情報の多くは **コンパイル時に消費** され、機械語生成に使われる
- 実行時に Python 式の **「Contact クラスオブジェクト」** は存在しない
- 実行時メモリにあるのは **機械語（コード領域）** と **インスタンスのデータ（スタック/ヒープ）**

### Python（インタプリタ）

```
import / class 定義
    ↓ 実行時
ヒープ上にクラスオブジェクト作成
    ↓
Contact() でインスタンスもヒープ上
```

---

## 10. メモリレイアウト

プログラム実行時、OS が実行ファイルをメモリに載せる。**機械語もメモリ上にある。**

```
プロセスのメモリ空間
├─ コード領域 (.text)       ← 関数の機械語
├─ 読み取り専用 (.rodata)
├─ データ (.data)            ← 初期値あり global/static
├─ BSS (.bss)                ← 0初期化 global/static
├─ ヒープ                    ← new（ex01 では基本使わない）
└─ スタック                  ← phoneBook, ローカル変数
```

### 「静的領域」の2つの意味（注意）

| 意味 | 含むもの |
|---|---|
| **広い意味**（スタック/ヒープ以外） | コード領域 + .data + .bss + .rodata など |
| **狭い意味**（C/C++ の `static` 変数） | .data + .bss のみ（global / static 変数） |

「コード領域は静的領域の一部？」→ **広い分類なら Yes、狭い分類（static 変数）なら No**。

### ex01 で各ものが置かれる場所

| もの | 領域 |
|---|---|
| `Contact::getFirstName` の機械語 | コード領域 |
| `PhoneBook phoneBook` | スタック |
| `_contacts[0]._firstName` など | スタック上（PhoneBook の一部） |
| `static` メンバ（あれば） | 狭義の静的領域 |

---

## 11. ex01 全体のタイムライン

```
① コンパイル時
   - Contact, PhoneBook の型情報をコンパイラが記録
   - メンバ関数を機械語に変換
   - インスタンスはまだ存在しない

② main 開始
   PhoneBook phoneBook;
   - スタックに PhoneBook 確保
   - Contact(void) × 8
   - _contactCount = 0, _oldestIndex = 0

③ ADD
   promptContact()
     - 5引数コンストラクタで Contact 作成
   addContact()
     - _contacts[i] = contact（代入）

④ SEARCH
   - getter で一覧表示
   - printDetails() で詳細表示

⑤ EXIT / main 終了
   - phoneBook のデストラクタ（暗黙）
   - スタック解放
```

---

## 12. 用語クイックリファレンス

| 用語 | 意味 |
|---|---|
| **クラス定義** | 設計図（型）。メモリ上の実体ではない |
| **インスタンス / オブジェクト** | 設計図から作られた実体 |
| **コンストラクタ** | インスタンス生成時の初期化処理 |
| **カプセル化** | private で隠し、public API 経由でアクセス |
| **値オブジェクト** | 作ったら変えず、差し替えで更新 |
| **RAII** | スコープ終了でリソース自動解放（C++ の基本） |
| **vtable** | virtual 関数用。ex01 では基本なし |
| **オーバーロード** | 同名・引数違いの関数/コンストラクタ |

---

## 13. 初期化リストとは（初期化 vs 代入）

### 構文

```cpp
Contact::Contact(const std::string& firstName, ...)
    : _firstName(firstName),   // ← 初期化リスト開始
      _lastName(lastName),
      ...
{ }                            // ← コンストラクタ本体
```

| 部分 | 意味 |
|---|---|
| `:` | 初期化リストの開始 |
| `_firstName(firstName)` | メンバー `_firstName` を、引数 `firstName` で**初期化** |
| `{ }` | コンストラクタ本体 |

左がメンバー変数、右が初期化に使う値（引数など）。

### 「代入」ではない

C++ ではメンバー変数はオブジェクト作成時に必ず一度「作られる」。その瞬間に値を渡すのが**初期化**。

#### パターンA：コンストラクタ本体で代入

```cpp
Contact::Contact(const std::string& firstName, ...) {
    _firstName = firstName;  // 代入
}
```

```
1. _firstName がデフォルトで作られる（空の string など）
2. そのあと firstName の値が代入される
```

#### パターンB：初期化リスト（推奨）

```cpp
Contact::Contact(const std::string& firstName, ...)
    : _firstName(firstName)
{ }
```

```
1. _firstName が firstName の値で直接作られる（1段階）
```

### 初期化リストを使う理由

| 理由 | 説明 |
|---|---|
| **効率** | `std::string` などは代入だと「一度作ってから書き換え」になる。初期化リストなら最初から正しい値で作れる |
| **必須の場合** | `const` メンバー、参照メンバーは代入できない。初期化リストでしか設定できない |
| **慣習・推奨** | C++ Core Guidelines C.49 などで「代入より初期化を優先」とされている（→ セクション15） |

### 覚え方

> コンストラクタの `{` の前の `:` は、「このメンバーはこの値で生まれてくる」と宣言している。

「生まれたあとで書き換える（代入）」ではなく、「生まれる瞬間からこの値（初期化）」。

---

## 14. 値オブジェクトと初期化リスト

### 値オブジェクト（E案）が求めること

| 原則 | 意味 |
|---|---|
| **setter なし** | 作ったあと個別フィールドを書き換えない |
| **生成時に全部決める** | 5項目をまとめてコンストラクタで渡す |
| **変更は差し替え** | 中身を直すのではなく、新しい `Contact` で上書き |

### 初期化リストは値オブジェクトにとって正しい

「値オブジェクトだから初期化リストはダメ」では**ない**。

- **初期化リスト** = オブジェクト**誕生時**の1回だけ
- **setter / 代入** = オブジェクト**存命中**の書き換え

値オブジェクトが避けたいのは後者。

```cpp
// 避けたいパターン
Contact c;
c.setFirstName("Alice");  // 後から少しずつ変更

// 正しいパターン（今のコード）
Contact c("Alice", "Smith", "Ally", "090-1234", "secret");
// 作った瞬間に全部決まっている → 初期化リストで実装
```

PhoneBook 側も「中身を編集」ではなく「丸ごと差し替え」:

```cpp
this->_contacts[this->_oldestIndex] = contact;
```

### デフォルトコンストラクタとの関係

| コンストラクタ | 役割 | 値オブジェクトとの関係 |
|---|---|---|
| 5引数 + 初期化リスト | 本物の連絡先を作る | ◎ 値オブジェクトの核心 |
| デフォルト（空） | 配列 `_contacts[8]` の枠確保 | △ 技術的な都合。ADD 後に上書きされる |

---

## 15. 初期化リストの公式ドキュメント

「メンバーには基本、初期化リストを使う」という説明の根拠。**ISO C++ 標準本文には「ベストプラクティス」とは書いていない**（言語仕様のみ）。推奨は別ドキュメントに記載。

| 資料 | URL | 言っていること |
|---|---|---|
| **C++ Core Guidelines C.49** | https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c49-use-initialization-rather-than-assignment-for-members | **推奨**: コンストラクタでは代入より初期化を優先 |
| **ISO C++ FAQ** | https://isocpp.org/wiki/faq/ctors#init-lists | **推奨**: 原則すべてのメンバーを初期化リストで初期化 |
| **Microsoft Learn** | https://learn.microsoft.com/en-us/cpp/cpp/constructors-cpp?view=msvc-170 | **推奨**: Prefer member initializer lists |
| **cppreference** | https://en.cppreference.com/w/cpp/language/initializer_list | **仕様**: 構文・意味・`const`/参照メンバーでは必須 |
| **C++ 標準草案** | https://eel.is/c++draft/class.base.init | **仕様**: 言語規則の定義（推奨の記述はなし） |
| **clang-tidy** | https://clang.llvm.org/extra/clang-tidy/checks/cppcoreguidelines/prefer-member-initializer.html | C.49 を静的解析でチェック |

### 区別

| 種類 | 内容 |
|---|---|
| **仕様**（cppreference / 標準草案） | 初期化リストとは何か、いつ必須か |
| **推奨**（Core Guidelines / FAQ / MSVC） | 代入より初期化リストを使え |

Core Guidelines C.49 の Good / Bad 例:

```cpp
// Good
A(czstring p) : s1{p} { }

// Bad
B(const char* p) { s1 = p; }  // デフォルト構築 → 代入
```

---

## 16. 初期化リストの書き方：`()` と `{}`

Core Guidelines の例 `: s1{p}` と、ex01 の `: _firstName(firstName)` は**同じ初期化リスト**で、括弧の種類が違うだけ。

cppreference ではメンバー初期化子に2種類:

| 書き方 | 名称 | C++98 |
|---|---|---|
| `メンバー(式)` | direct-initialization | ✅ |
| `メンバー{式}` | list-initialization | ❌（C++11〜） |

ex01 の Makefile:

```makefile
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
```

**C++98 では `{}` による初期化は使えない**ため、`()` 形式が正しい。

Microsoft Learn や ISO C++ FAQ も `()` 形式の例を載せている:

```cpp
Box(int width, int length, int height)
    : m_width(width), m_length(length), m_height(height)
{}

Fred::Fred() : x_(whatever) { }
```

| 項目 | Core Guidelines 例 | ex01 の Contact |
|---|---|---|
| 初期化リストを使う | ✅ | ✅ |
| 括弧 | `{}`（C++11+） | `()`（C++98 OK） |
| やっていること | メンバーを初期化 | メンバーを初期化 |

---

## 17. Makefile

```makefile
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = phonebook
SRCS = main.cpp Contact.cpp PhoneBook.cpp
OBJS = $(SRCS:.cpp=.o)
```

| 行 | 意味 |
|---|---|
| `-Wall -Wextra -Werror` | 警告をすべて表示し、警告があればエラー扱い |
| `-std=c++98` | C++98 標準（42 課題の要件） |
| `OBJS = $(SRCS:.cpp=.o)` | `.cpp` を `.o` に置き換え |
| `all` | デフォルトターゲット。`make` で `phonebook` をビルド |
| `clean` / `fclean` / `re` | オブジェクト削除 / 実行ファイルも削除 / 再ビルド |
| `.PHONY` | `all` などはファイル名ではないターゲット |

```
main.cpp + Contact.cpp + PhoneBook.cpp
    ↓ コンパイル
main.o + Contact.o + PhoneBook.o
    ↓ リンク
phonebook（実行ファイル）
```

---

## 18. PhoneBook クラス

### 宣言（PhoneBook.hpp）

```cpp
class PhoneBook {
private:
    static const int MAX_CONTACTS = 8;
    Contact _contacts[MAX_CONTACTS];
    int _contactCount;
    int _oldestContactIndex;
public:
    void addContact(const Contact& contact);
    void searchContact(void) const;
};
```

### 匿名名前空間のヘルパー

| 関数 | 役割 |
|---|---|
| `_truncateField` | 10文字超を切り詰め + `.` |
| `_printField` | `setw(10)` + `right` で右寄せ表示 |
| `_getArrayIndex` | 表示 index → 配列 index（`maxContacts` は引数で渡す） |

### addContact

```cpp
this->_contacts[this->_oldestContactIndex] = contact;
this->_oldestContactIndex = (this->_oldestContactIndex + 1) % MAX_CONTACTS;
if (this->_contactCount < MAX_CONTACTS)
    this->_contactCount++;
```

---

## 19. main.cpp と入出力

### readNonEmptyLine

空でない1行を読む。課題では空フィールド不可。

```cpp
static std::string readNonEmptyLine(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line))
            exit(0);           // EOF（Ctrl+D 等）
        if (!line.empty())
            return line;
    }
}
```

### promptContact

5項目を入力し、5引数コンストラクタで `Contact` を作って返す。

### コマンドループ

| コマンド | 処理 |
|---|---|
| `ADD` | 5項目入力 → `addContact()` |
| `SEARCH` | 一覧・index 入力・詳細表示 |
| `EXIT` | ループ終了 |
| その他 | 無視 |

**現在の main** はコマンドも `getline` で統一（`>>` + `ignore` は不使用）。

### std::cin.ignore が必要なケース（参考）

`>>` と `getline` を混ぜる場合、改行 `\n` がバッファに残る。

```cpp
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
```

SEARCH の index 入力（`cin >>`）後にも同様の問題が起きうる → セクション29参照。

---

## 20. プログラム全体の流れ（実行例）

```bash
make          # ビルド
./phonebook   # 実行
```

```
Enter a command: ADD
Enter first name: Alice
...
Enter a command: SEARCH
     index|first name| last name|  nickname|
         0|     Alice|     Smith|      Ally|
Index: 0
First name: Alice
...
Enter a command: EXIT
```

### ADD の流れ

```
1. ユーザーが ADD 入力（getline）
2. promptContact() で5項目入力 → Contact 作成
3. addContact() で _contacts[oldestContactIndex] に代入
4. oldestContactIndex, contactCount 更新
```

### 設計の採用方針（architecture.md より）

**E案（値オブジェクト）+ D案（printDetails）** を採用:

| 部分 | 案 | 用途 |
|---|---|---|
| コンストラクタ生成 | E案 | ADD |
| 個別 getter | E案 | SEARCH 一覧 |
| printDetails | D案 | SEARCH 詳細 |

| クラス | 性質 | 変更方法 |
|---|---|---|
| Contact | 値オブジェクト寄り | 新インスタンス作成・差し替え |
| PhoneBook | エンティティ寄り | 同じインスタンスを更新 |

---

## 21. getter の `std::string` と末尾の `const`

```cpp
std::string getFirstName(void) const;
//^^^^^^^^^                      ^^^^^
// 戻り値の型                     const メンバー関数
```

| 部分 | 意味 |
|---|---|
| 先頭の `std::string` | 戻り値の型（名前を**コピー**で返す） |
| 末尾の `const` | この関数はオブジェクトを**変更しない** |

### 3種類の `const`（混同注意）

| 位置 | 例 | 意味 |
|---|---|---|
| 型の前 | `const std::string _x` | メンバー自体を後から変更不可 |
| 引数 | `const std::string& x` | 引数を書き換えない |
| 関数末尾 | `getFirstName() const` | 関数内でメンバーを変更しない |

### const メンバー関数の呼び出し

| オブジェクト | `getFirstName() const` | 非 const メンバー関数 |
|---|---|---|
| 非 const | ✅ | ✅ |
| `const Contact&` | ✅ | ❌ コンパイルエラー |

典型エラー:

```
error: 'this' argument ... has type 'const Contact', but function is not marked const
```

---

## 22. const メンバー変数 + setter + PhoneBook の `=`

### const メンバー + setter

```cpp
const std::string _firstName;
void setFirstName(...) { _firstName = ...; }  // ❌ コンパイルエラー
```

`const` メンバーは生成後に代入不可。setter は設計として矛盾。

### PhoneBook の `_contacts[i] = contact` は「置き換え」ではない

C++ では `=` は **既存オブジェクトへの代入**（`operator=`）。

```
設計の言葉:  差し替え（replace）
C++ の = :  代入（assign into existing object）
```

| 方法 | const メンバー | ex01 |
|---|---|---|
| `const` メンバー + `=` | ❌ コンパイル不可 | — |
| 非 const メンバー + setter なし + `=` | ✅ | **現行の落としどころ** |
| placement new | ✅ 可能 | 過剰 |

---

## 23. 宣言と定義の一致

`.hpp` と `.cpp` のシグネチャは**完全一致**が必要。

```cpp
// .hpp
Contact(const std::string& firstName, ...);

// .cpp  ❌
Contact(std::string& firstName, ...);  // const の有無で別物
```

---

## 24. `.dio` ファイルについて

`ex01.dio` は **draw.io の図ファイル（XML）**。実行ファイルではない。

```bash
./ex01.dio   # ❌ bash が XML をコマンドとして解釈してエラー
./phonebook  # ✅ make 後の実行ファイル
```

---

## 25. SEARCH 課題要件

### 第1段階：一覧（4列）

| 列 | index / first name / last name / nickname |
|---|---|
| 列幅 | 10文字 |
| 区切り | `\|` |
| 配置 | 右寄せ |
| 長い場合 | 切り詰め + 末尾 `.` |
| ライブラリ | `iomanip` 使用が期待される |

### 第2段階：index 入力 → 詳細

- 5フィールドを1行ずつ表示
- 範囲外 index の動作は自分で定義（サイレント return も可）

---

## 26. `_truncateField` と `std::setw(10)` の役割分担

| 関数 | 役割 |
|---|---|
| `_truncateField` | **最大**10文字（長い場合 9文字 + `.`） |
| `std::setw(10)` + `std::right` | **幅10の場**を確保（短い場合スペース埋め + 右寄せ） |

**重複ではない。** 前者は上限、後者は下限・レイアウト。

```
"Alice"（5文字）→ truncate: "Alice" → setw: "     Alice"
"VeryLong..."   → truncate: "VeryLongN." → setw: そのまま10文字
```

---

## 27. ストリーム変換（C++98）

### int → string（一覧の index 列）

```cpp
std::ostringstream indexStream;
indexStream << i;
indexStream.str();  // "0", "1", ...
```

- `std::to_string(i)` は **C++11〜** → ex01 では不可
- `<< i` は「代入」ではなくストリームへの**出力**

### ostringstream / istringstream / stringstream

| 型 | 方向 | 用途 |
|---|---|---|
| `ostringstream` | 書く | 数値 → 文字列 |
| `istringstream` | 読む | 文字列 → 数値 |
| `stringstream` | 両方 | 使えるが ex01 では o/i で十分 |

### istringstream はキャストではない

```cpp
std::istringstream indexStream(indexInput);
```

`indexInput` を **コンストラクタの初期値**として渡し、新しいストリームオブジェクトを**生成**している。型キャストではない。

---

## 28. 入力設計：getline vs `cin >>`

| 入力 | 推奨 | 理由 |
|---|---|---|
| コマンド・名前（ADD） | `getline` | スペースを含む1行 |
| index（SEARCH） | `cin >>` + `ignore` | 数値1つ。意図が明確 |

### `cin >>` と `getline` の違い

| | `cin >>` | `getline` |
|---|---|---|
| 区切り | 空白・改行 | 改行のみ |
| スペースを含む | ❌ 1単語だけ | ✅ |

### index を getline + istringstream にする必要は？

`getline` で string を取った以上、string → int 変換は必要。  
ただし **index だけなら `cin >>` の方が短く、ex01 ではこちらを採用**。

---

## 29. `cin >>` 後の `ignore`

### 問題

`main` が `getline`、`SEARCH` が `cin >>` のとき:

```
Index: 0↵
>> が 0 だけ読む → バッファに \n 残留
次の getline が空行を読む ❌
```

### 対策

```cpp
if (!(std::cin >> indexInput))
    return;
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
```

### `ignore()` 1文字 vs `ignore(max, '\n')`

| 入力 | `ignore()` | `ignore(max, '\n')` |
|---|---|---|
| `0↵` | ✅ | ✅ |
| `0 ↵` | ❌ `\n` 残留 | ✅ |
| `0 abc↵` | ❌ ゴミ残留 | ✅ |

ex01 の通常入力（数字 + Enter）なら `ignore()` でも可。堅牢には `ignore(max, '\n')`。

---

## 30. `static MAX_CONTACTS` と `this->`

```cpp
class PhoneBook {
    static const int MAX_CONTACTS = 8;  // クラス全体で1つ
    int _oldestContactIndex;            // インスタンスごと
};
```

| メンバー | `this->` |
|---|---|
| `_oldestContactIndex` | 必要（インスタンスごと） |
| `MAX_CONTACTS` | 不要（クラス共有。`PhoneBook::MAX_CONTACTS` とも書ける） |

### 匿名 namespace から private メンバーに触れない

```cpp
namespace {
    int _getArrayIndex(..., int maxContacts) { ... }  // 引数で受け取る
}
// 呼び出し側（メンバー関数内）
_getArrayIndex(i, _contactCount, _oldestContactIndex, MAX_CONTACTS);
```

---

## 31. 課題レビューで修正した点

| 問題 | 修正 |
|---|---|
| `<algorithm>` の `std::min` | Module 00 禁止 → `if (_contactCount < MAX_CONTACTS)` |
| `iomanip` 未使用 | `_printField` + `setw` / `right` |
| `std::int` | 存在しない型 → `int` |
| 匿名 namespace から `MAX_CONTACTS` | `maxContacts` 引数で渡す |
| `getline` + `cin >>` 混在 | index 後に `cin.ignore(..., '\n')` |
| `#include <limits>` | `numeric_limits` 用（SEARCH index 後） |

### Module 00 で避けるもの

- `<algorithm>`（`std::min` 等）
- `*printf()`, `*alloc()`, `free()`
- `using namespace`
- `new` / 動的確保（ex01）

---

## 32. searchContact 処理フロー

```
1. _contactCount == 0 → return
2. ヘッダ4列（_printField + |）
3. for i = 0 .. count-1
     _getArrayIndex(i, ...) で配列 index 取得
     ostringstream で i → 文字列
     4列表示
4. "Index: " → cin >> indexInput → ignore
5. 範囲外 → return
6. _getArrayIndex(indexInput, ...) → printDetails()
```
