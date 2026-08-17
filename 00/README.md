# CPP Module 00 — 何を学んだか

後日この課題を見返したときに、文法の要点と当時考えたことを思い出すためのメモ。
課題の手順書ではない。

詳細な実装メモは `ex01/Learning.md`。クラス構成は `ex01/architecture.md`。

---

## 目次

1. [ディレクトリ構成](#1-ディレクトリ構成)
2. [課題ごとの到達点](#2-課題ごとの到達点)
3. [クラス設計 — `.hpp` と `.cpp`](#3-クラス設計--hpp-と-cpp)
4. [コンストラクタ / デストラクタ / RAII](#4-コンストラクタ--デストラクタ--raii)
5. [初期化リスト](#5-初期化リスト)
6. [const](#6-const)
7. [static](#7-static)
8. [static const int vs 非 const static](#8-static-const-int-vs-非-const-static)
9. [入出力](#9-入出力)
10. [Makefile](#10-makefile)
11. [課題の読み方（ex02）](#11-課題の読み方ex02)
12. [tests.cpp で読んだ STL](#12-testscpp-で読んだ-stl)
13. [評価で聞かれやすいこと](#13-評価で聞かれやすいこと)
14. [Module 01 に向けて](#14-module-01-に向けて)
15. [細かいが効く理解](#15-細かいが効く理解)
16. [課題中に悩んだこと](#16-課題中に悩んだこと)

---

## 1. ディレクトリ構成

```
00/
├── ex00/   megaphone   — cout、argc/argv、文字変換
├── ex01/   phonebook   — クラス、固定配列、getline
├── ex02/   Account     — static、ログ復元、オブジェクトの生成と破棄
└── README.md
```

---

## 2. 課題ごとの到達点

| 課題 | 作ったもの | 学んだこと |
|------|------------|------------|
| **ex00** | 引数を大文字にして標準出力へ出す | `std::cout`、`argc` / `argv`、C++ のプログラムの形 |
| **ex01** | 最大8件の電話帳 | クラスを hpp/cpp に分ける、初期化リスト、const、固定配列 |
| **ex02** | `Account.cpp` の実装 | static メンバー、コンストラクタとデストラクタ、log から仕様を読む |

ex02 は Module 00 の合格に必須ではない。static と「オブジェクトがいつ作られ、いつ破棄されるか」を確認する課題。

---

## 3. クラス設計 — `.hpp` と `.cpp`

### 用語

- **宣言:** 名前と型をコンパイラに知らせる。中身は書かない。
- **定義:** 実体を書く。関数なら `{ }` の中身、変数ならメモリ上の実体。

### 文法

クラスは通常、次の2ファイルに分ける。

| ファイル | 書くこと |
|----------|----------|
| `.hpp` | クラスの宣言。メンバー変数と、関数の宣言 |
| `.cpp` | 各メンバー関数の定義 |

```cpp
// Contact.hpp — 宣言。この関数がある、と知らせる
std::string getFirstName(void) const;

// Contact.cpp — 定義。実際の処理
std::string Contact::getFirstName(void) const {
    return this->_firstName;
}
```

`Contact::` は「Contact クラスのメンバーである」という意味。

宣言と定義は、戻り値・引数・`const` の有無まで同じにする。違うとコンパイルエラーになる。

`class Account { ... };` は hpp に1回だけ書く。cpp に同じ class を書くと、同じクラスを2回定義したことになりエラーになる。

### 考えたこと

ex02 で最初、`Account.cpp` に hpp の `class Account { ... }` をコピーした。これは定義ではなく、宣言の重複になる。cpp に書くのは `Account::makeDeposit(...) { ... }` のような関数の中身。

ex01 の Contact には setter を置かなかった。連絡先の内容を後から変える関数はない。新しい Contact をコンストラクタで作り、PhoneBook が `_contacts[i] = contact` でコピーする。

---

## 4. コンストラクタ / デストラクタ / RAII

### 用語

- **コンストラクタ:** オブジェクトが作られるときに自動で呼ばれる関数。クラス名と同じ名前。戻り値はない。
- **デストラクタ:** オブジェクトが破棄されるときに自動で呼ばれる関数。名前は `~クラス名`。
- **スコープ:** `{ }` で囲まれた範囲。関数の終わり、ブロックの終わりなど。
- **RAII:** オブジェクトの生成時にコンストラクタ、破棄時にデストラクタが必ず走る、という C++ の仕組み。確保と解放をこの2つに対応させる。

### 文法

オブジェクトの作り方は2つある。どちらもコンストラクタを呼ぶ。

```cpp
Contact c("A", "B", "C", "D", "E");  // 変数 c がある。c がスコープを抜けるまで残る
return Contact("A", "B", "C", "D", "E");  // 変数名がない。return や引数渡しに使う
```

デストラクタが呼ばれる例:

- 関数が終わる（ローカル変数）
- `std::vector<Account>` が破棄される（中の Account も破棄される）

ex02 では、コンストラクタが `created` を出力し、デストラクタが `closed` を出力する。`main` が終わると `accounts` が破棄され、Account のデストラクタが8回呼ばれる。

### 考えたこと

`PhoneBook phoneBook;` と書いた時点で、メンバー `Contact _contacts[8]` の各要素に、引数なしのコンストラクタ（デフォルトコンストラクタ）が8回呼ばれる。ADD するまで Contact が存在しない、ではない。

引数5つのコンストラクタだけを定義すると、`Contact _contacts[8];` はコンパイルエラーになる。配列の各要素は、引数なしで作られるため。

ADD の `_contacts[i] = contact` はコンストラクタではない。すでに存在する `_contacts[i]` へ、代入演算子でコピーする。

---

## 5. 初期化リスト

### 用語

- **初期化:** オブジェクトが作られるときに、最初の値を与えること。
- **代入:** すでに存在するオブジェクトへ、あとから値を入れること。`=` を使う。

### 文法

コンストラクタの `{ }` の前に、`: メンバー(値)` と書く。これが初期化リスト。

```cpp
Account::Account(int initial_deposit)
    : _accountIndex(_nbAccounts),
      _amount(initial_deposit),
      _nbDeposits(0),
      _nbWithdrawals(0)
{
    _nbAccounts++;  // 既存の static 変数を更新している。初期化ではない
}
```

| | 初期化リスト `: _x(v)` | コンストラクタ本体 `_x = v` |
|--|------------------------|------------------------------|
| いつ | メンバーが作られるとき | メンバーが作られたあと |
| const メンバー | これでしか値を入れられない | できない |
| 参照メンバー | これでしか束縛できない | できない |

`std::string` を本体で `_name = s;` すると、先に空の string が作られ、そのあと代入される。初期化リストなら、最初から `s` で作られる。

### 考えたこと

Contact の `_firstName` などを `const std::string` にすると、作ったあとに値を変えられない。しかし PhoneBook は `_contacts[i] = contact` で代入する。const メンバーがあると代入できない。そのためメンバーは非 const の `std::string` にした。

---

## 6. const

### 文法

`const` は「変更しない」ことをコンパイラに伝える。置く位置で対象が変わる。

```cpp
void displayStatus(void) const;           // この関数は、呼び出したオブジェクトのメンバーを変えない
void addContact(const Contact& contact);  // contact をコピーせず、contact の中身も変えない
```

| 書き方 | 対象 | 効果 |
|--------|------|------|
| 関数の `)` の後ろの `const` | そのオブジェクト | メンバーを変更するコードを書くとエラー |
| `const Contact&` | 引数 | 参照で受け取る（コピーしない）。引数経由で変更できない |
| メンバー変数の `const` | そのメンバー | そのオブジェクトの寿命中、値を変えられない |

関数末尾が `const` のとき、その関数内の `this` の型は `const クラス名*` になる。

### 考えたこと

getter、`printDetails`、`searchContact` はメンバーを変えないので末尾に `const` を付けた。`addContact` は `_contacts` を変えるので付けない。

---

## 7. static

### 用語

- **インスタンスメンバー:** オブジェクトごとに1つある変数・関数。`_amount` など。
- **static メンバー:** クラスに1つだけある変数・関数。オブジェクトが何個あっても共有される。

### 文法

```cpp
// Account.hpp — 宣言。こういう変数がある、と知らせるだけ
static int _nbAccounts;

// Account.cpp — 定義。実体を1つ作り、初期値を入れる
int Account::_nbAccounts = 0;
```

定義はプログラム開始時に1回だけ行われる。

コンストラクタの中で `_nbAccounts = 0;` としてはいけない。コンストラクタは Account が作られるたびに呼ばれる。8個作ると8回 `0` が入り、口座数が正しく増えない。

static メンバー関数は、オブジェクトなしで `Account::displayAccountsInfos()` のように呼べる。`this` はない。

### 考えたこと

Account が8個あっても `_nbAccounts` は1つ。口座数・合計残高・入金回数合計・出金回数合計が static。各口座の残高 `_amount` はインスタンスメンバー。

---

## 8. static const int vs 非 const static

C++98 では、static メンバーの書き方が `const` の有無で違う。

| | ex01 `MAX_CONTACTS` | ex02 `_nbAccounts` |
|--|---------------------|---------------------|
| 宣言 | `static const int MAX_CONTACTS = 8;` | `static int _nbAccounts;` |
| 初期化の場所 | hpp のクラス内でよい | cpp で定義する |
| 実行中の値 | 変わらない | `++` などで変わる |
| 用途 | 配列の要素数 `Contact _contacts[MAX_CONTACTS]` | 現在の口座数 |

hpp を編集できないから cpp に書く、だけではない。非 const の static は、言語の規則として cpp 側の定義が必要。

`static const int MAX_CONTACTS = 8;` は正しい書き方。

---

## 9. 入出力

### 文法

`std::cout` と `std::cin` は、標準出力・標準入力に読み書きするためのオブジェクト。`<<` と `>>` は、それらに対する演算子。文字列同士を `+` でつなぐ処理ではない。

```cpp
std::cout << "hello" << std::endl;     // 標準出力へ出す
std::getline(std::cin, line);          // 改行まで1行読む。空白も含む
std::cin >> index;                     // 空白で区切られた1つの値を読む
std::cin.ignore(..., '\n');            // >> のあと、行末の改行が残るので捨てる
```

幅と埋め文字（`<iomanip>`）:

| 関数 | 効果 |
|------|------|
| `std::setw(2)` | 次に出力する **1つだけ**、幅を2にする |
| `std::setfill('0')` | 幅に足りないとき埋める文字を `'0'` にする。次に変えるまで有効 |

`setfill('0')` だけでは幅がないので、`4` は `"4"` のまま。`setw(2)` と組み合わせて `"04"` になる。`setfill` と `setw` の書く順番は、この用途では結果が同じ。

時刻（`<ctime>`）:

```cpp
std::time_t now = std::time(0);     // 現在時刻を秒の整数で得る
std::tm *ltm = std::localtime(&now); // 年・月・日・時・分・秒に分ける。実行環境のタイムゾーン
```

`tm_year` に入っているのは西暦そのものではない。1900 年からの差。1992 年なら `92`。表示は `1900 + ltm->tm_year`。  
`tm_mon` は 0 が1月、11 が12月。表示は `1 + ltm->tm_mon`。

### 考えたこと

ex01 のコマンドと ADD の各項目は `getline` にした。`cin >>` は空白で切れる。空行の扱いも違う。SEARCH の index だけ `cin >>` のあと `ignore` で改行を捨てた。

`std::cout << std::setfill('0') << std::setw(2) << ltm->tm_mday` は、`'0'` と整数を結合しているのではない。`cout` の設定を変え、その設定で整数を出力している。

---

## 10. Makefile

コンパイル手順をファイルに書いたもの。42 では次が基本。

```makefile
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = phonebook
SRCS = main.cpp Contact.cpp PhoneBook.cpp
```

| 項目 | 意味 |
|------|------|
| `-Wall -Wextra -Werror` | 警告を出し、警告をエラーとして扱う |
| `-std=c++98` | C++98 の文法だけ使う |
| `all` | 実行ファイルを作る |
| `clean` | `.o` を消す |
| `fclean` | `.o` と実行ファイルを消す |
| `re` | `fclean` のあと `all` |

ex02 の `SRCS` は `Account.cpp tests.cpp`。`Account.hpp` と `tests.cpp` は課題で渡されたファイルなので編集しない。

---

## 11. 課題の読み方（ex02）

渡されるもの: `Account.hpp`、`tests.cpp`、ログファイル。  
自分が書くもの: `Account.cpp` だけ。

手順:

1. `tests.cpp` が、どの関数をどの順で呼んでいるかを読む。
2. ログの各行が、どの関数の出力かを対応させる。
3. ログの項目名から、その関数が何をするかを決める。例: `p_amount` は更新前の残高。`withdrawal:refused` は出金せずに終わる。

実行順:

1. `std::vector<Account>` の生成 → `Account(int)` が8回 → ログ `created`
2. `displayAccountsInfos` と各口座の `displayStatus`
3. `makeDeposit` が8回
4. 再度 display
5. `makeWithdrawal` が8回（残高不足なら `refused`）
6. 再度 display
7. `main` 終了 → デストラクタが8回 → ログ `closed`

タイムスタンプ `[YYYYMMDD_HHMMSS]` は実行時刻で違ってよい。それ以外の文字列はログと一致させる。

---

## 12. tests.cpp で読んだ STL

STL は標準ライブラリのコンテナやアルゴリズムの総称。

Module 00 では、自分のコード（`Account.cpp` など）に STL を使わない制限があることが多い。`tests.cpp` は課題側が書いたファイルなので、`vector` などが使われている。読めることと、自分の提出コードで使うことは別。

### `std::vector<T>`

`T` 型の要素を、実行時に個数を変えられる列として持つクラス。C の配列 `T a[8]` は要素数がコンパイル時に固定。`vector` は固定ではない。

for 文ではない。要素を保持する。生成や走査のときに内部で繰り返し処理が走る。

### 範囲を渡すコンストラクタ

```cpp
accounts_t accounts(amounts, amounts + amounts_size);
```

`accounts_t` は `std::vector<Account>` の別名。  
この行は **vector のコンストラクタ** を呼ぶ。引数は2つで、どちらも `int` 配列上のアドレス。

- `amounts` … 先頭要素のアドレス
- `amounts + amounts_size` … 末尾の次のアドレス（その位置の要素は使わない）

vector のコンストラクタは、この範囲の各 `int` について `Account(そのint)` を呼ぶ。`Account` のコンストラクタの引数は常に `int` 1つ。2引数を受け取っているのは vector。

`accounts(...)` という名前の関数はない。`型名 変数名(引数)` は、その型のオブジェクトを作る書き方。

### `Account::t`

```cpp
typedef Account t;
```

`t` は `Account` と同じ型を指す名前。`Account::t` と `Account` は同じ。

### iterator

vector の中の、ある要素の位置を表す型。ポインタに近い使い方をする。

- `accounts.begin()` … 先頭要素の位置
- `accounts.end()` … 末尾の次。末尾の要素そのものではない

`begin` と `end` は、同じ vector から取る。型が違う iterator を混ぜるとコンパイルエラー。

### `std::pair`

2つの値を、`first` と `second` という名前で持つ型。tests.cpp では、口座の iterator と金額の iterator を1つの変数に入れ、両方を1つずつ進める。

### `std::for_each`

第1引数から第2引数の直前まで、各要素に第3引数の処理を行う。

```cpp
std::for_each(acc_begin, acc_end, std::mem_fun_ref(&Account::displayStatus));
```

各 `Account` に対して `displayStatus()` を呼ぶ。  
`acc_begin` と `dep_end` のように型が違う iterator を渡すとエラー。  
`dep_begin` から `dep_end` は `int` の列なので、`Account::displayStatus` は呼べない。

`std::mem_fun_ref` は、メンバー関数へのポインタを `for_each` が呼べる形に変換する（C++98）。

---

## 13. 評価で聞かれやすいこと

| トピック | 答えの要点 |
|----------|------------|
| 宣言 vs 定義 | 宣言は名前と型。定義は実体。hpp に宣言、cpp に定義。非 const の static 変数は cpp で定義する |
| const メンバー関数 | そのオブジェクトのメンバーを変更しない。末尾に `const` |
| `const T&` 引数 | 参照なのでコピーしない。`const` なので関数内で変更できない |
| 初期化リストが必要な理由 | const メンバーと参照メンバーは、作られるときに値を決める必要がある。代入では無理 |
| 代入 vs 初期化 | `_contacts[i] = contact` は代入。`_contacts[i]` は `PhoneBook` 生成時にすでにコンストラクタ済み |
| デストラクタがいつ呼ばれるか | スコープ終了、vector の破棄など。ex02 の `closed` |
| `this` | メンバー関数内で、呼び出されたオブジェクトへのポインタ。`this->_firstName`。static 関数には `this` がない |

確認用:

1. static メンバーとインスタンスメンバーの違いは何か。
2. `static const int` は hpp で初期化でき、`static int` は cpp が必要なのはなぜか。
3. メンバー関数の末尾の `const` は何を禁止するか。
4. `: _amount(x)` と `{ _amount = x; }` の違いは何か。
5. hpp と cpp に分ける理由は何か。
6. ex02 の `closed` はどのタイミングで出るか。
7. `getline` と `cin >>` は何が違うか。

---

## 14. Module 01 に向けて

| トピック | Module 00 で触れたこととの関係 |
|----------|--------------------------------|
| コピーコンストラクタ / 代入演算子 | `_contacts[i] = contact` で代入が起きている。ポインタをメンバーに持つと、同じ領域を2オブジェクトが指す問題が出る |
| ポインタ / 参照 / 配列 | ex01 の `Contact[8]`。ex02 の iterator は位置を指す |
| `new` / `delete` | ex01 では動的確保が禁止。01 以降で使う |
| オペレータオーバーロード | `<<` は `cout` に対して定義された演算子。整数と文字を結合する演算子ではない |

---

## 15. 細かいが効く理解

| トピック | 要点 |
|----------|------|
| `const` vs `#define` | 型がある定数には `const` または `static const int` を使う |
| ヘッダガード | `#ifndef` / `#define` / `#endif`。同じ hpp が2回 include されても、クラス定義が1回になる。`#pragma once` も同じ目的 |
| `explicit` コンストラクタ | 引数1つのコンストラクタが、意図しない型変換に使われるのを防ぐ。Module 01 以降 |
| アクセス指定子 | `private` の `Account(void)` があるため、クラスの外から `Account a;` と書けない |
| 未初期化変数 | 初期値を与えないと、中身は不定。`-Werror` でエラーになることがある。初期化リストで値を入れる |
| 名前空間 | `std::` は標準ライブラリの名前が属する名前空間。同じ名前の衝突を避ける。この課題では自分で `namespace` を定義していない |

---

## 16. 課題中に悩んだこと

詳細は上の各節。

| 悩み | 結論 |
|------|------|
| Account.cpp に class をコピーすれば復元か | 違う。hpp は宣言。cpp には `Account::関数名 { }` を書く |
| `accounts(amounts, amounts+n)` は Account に配列を渡しているか | 渡している先は vector のコンストラクタ。Account には各 `int` が1つずつ渡る |
| `accounts()` という関数があるか | ない。`accounts_t accounts(...)` は変数 `accounts` の定義 |
| Account のコンストラクタは1引数なのに2引数では | 2引数は vector 用。Account は `Account(int)` が要素の数だけ呼ばれる |
| vector は for と同じか | 違う。要素を保持するクラス。コンストラクタの内部で繰り返し `Account(int)` を呼ぶ |
| `Account::t` は何か | `typedef Account t;` なので `Account` と同じ型 |
| static をコンストラクタで 0 にしてよいか | よくない。オブジェクトの数だけ 0 が入る |
| `MAX_CONTACTS = 8` は間違いか | 間違いではない。`static const int` は hpp で初期化できる |
| `tm_year` をそのまま出せばよいか | 西暦ではない。`1900 + tm_year` が西暦 |
| `setfill('0') << 整数` は結合か | 結合ではない。出力ストリームの設定と、整数の出力 |
| Contact のメンバーを const にできるか | `_contacts[i] = contact` が代入なので、const メンバーだとできない |
| デフォルトコンストラクタは不要か | `Contact _contacts[8]` の各要素を作るために必要 |

---

## 関連ファイル

| ファイル | 内容 |
|----------|------|
| `ex01/Learning.md` | ex01 の詳細ノート |
| `ex01/architecture.md` | PhoneBook のクラス構成 |
| `ex02/19920104_091532.log` | ex02 の期待出力 |
