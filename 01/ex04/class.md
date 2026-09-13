# クラス化（オブジェクト指向）は何のためにあるのか

ex04 で「置換処理をクラスにすべきか」を考えた際に整理した内容。
クラスを使う／使わないの判断基準を後から思い出すためのメモ。

例には Module 00 / 01 で自分が書いた `PhoneBook`、`Account`、`Weapon`、`HumanB` を使う。

---

## 目次

1. [クラス化が叶えたい6つの目的](#クラス化が叶えたい6つの目的)
2. [目的1: 不変条件を保護する（カプセル化）](#目的1-不変条件を保護するカプセル化)
3. [目的2: 不変条件の確立と後片付けを自動化する（コンストラクタ・デストラクタ・RAII）](#目的2-不変条件の確立と後片付けを自動化するコンストラクタデストラクタraii)
4. [目的3: 実装を隠して変更の影響範囲を閉じ込める（情報隠蔽）](#目的3-実装を隠して変更の影響範囲を閉じ込める情報隠蔽)
5. [目的4: 概念に型を与えて間違いをコンパイラに検出させる（抽象データ型）](#目的4-概念に型を与えて間違いをコンパイラに検出させる抽象データ型)
6. [目的5: 呼び出し側を変えずに振る舞いを差し替える（多態性）](#目的5-呼び出し側を変えずに振る舞いを差し替える多態性)
7. [目的6: 状態と操作をまとめる（凝集）](#目的6-状態と操作をまとめる凝集)
8. [よくある誤解](#よくある誤解)
9. [判断基準](#判断基準)
10. [42 のカリキュラムとの対応](#42-のカリキュラムとの対応)
11. [ex04 での結論](#ex04-での結論)
12. [参考資料](#参考資料)

---

## クラス化が叶えたい6つの目的

重要な順に並べると次の6つ。

1. **不変条件を保護する**（カプセル化）
2. **不変条件の確立と後片付けを自動化する**（コンストラクタ／デストラクタ、RAII）
3. **実装を隠して、変更の影響範囲を閉じ込める**（情報隠蔽）
4. **概念に型を与えて、間違いをコンパイラに検出させる**（抽象データ型）
5. **呼び出し側を変えずに振る舞いを差し替える**（多態性）
6. **状態と操作をまとめる**（凝集）

**1〜5 はクラスでしか（あるいはクラスが圧倒的に得意に）実現できないこと。**
6 は C++ では名前空間やファイル分割でも代替できる。

よく言われる「まとめて整理できる」は、実は一番弱い理由。

---

## 目的1: 不変条件を保護する（カプセル化）

### 用語

**不変条件（invariant）** … そのオブジェクトが存在している間、**常に成り立っていなければならない条件**。これが崩れたオブジェクトは「壊れている」状態。

**カプセル化（encapsulation）** … データを `private` にし、決められた `public` 関数からしか触れないようにすること。

多くの入門書は「カプセル化とはデータを隠すこと」で説明を終えるが、**なぜ隠すのか** が本質。
答えは **不変条件を壊せる経路を減らすため**。

### `PhoneBook` で見る

`00/ex01/PhoneBook.hpp`

```cpp
class PhoneBook {
private:
	static const int MAX_CONTACTS = 8;
	Contact _contacts[MAX_CONTACTS];
	int _contactCount;
	int _oldestContactIndex;

public:
	PhoneBook(void);
	void addContact(const Contact& contact);
	void searchContact(void) const;
};
```

このクラスの不変条件。

- `0 <= _contactCount <= 8`
- `0 <= _oldestContactIndex < 8`
- `_contacts[0]` から `_contacts[_contactCount - 1]` までが有効な連絡先である
- `_oldestContactIndex` は、次に上書きすべき位置を正しく指している

**この4つは互いに関係している。** `_contactCount` だけを勝手に増やすと、`_contacts` の中身は空なのに「7件ある」ことになり、`searchContact()` がゴミを表示する。

もしこれらが `public` だったら、こう書けてしまう。

```cpp
PhoneBook book;
book._contactCount = 100;      // 配列は8個しかないのに
book._oldestContactIndex = -5; // 負のインデックス
book.searchContact();          // クラッシュ、または意味不明な出力
```

`private` にすると、この操作は **コンパイルエラー** になる。

### 本当の利益: バグを探す範囲が狭まる

ここが最も実用的な効果。

`_contactCount` が `public` の場合、プログラムが5万行あるなら **5万行のどこでも** `_contactCount` を壊せる。バグが起きたとき、疑うべき場所が5万行ある。

`private` の場合、`_contactCount` を書き換えられるのは **`PhoneBook` のメンバ関数だけ**。つまり `PhoneBook.cpp` の中だけ。不変条件が壊れていたら、犯人はそのファイルの中にいる。

```
public  → 犯人はプログラム全体のどこかにいる（捜索範囲: 全コード）
private → 犯人は PhoneBook.cpp の中にいる（捜索範囲: 数十行）
```

これは **局所的に推論できる（local reasoning）** という性質。
「このクラスの中だけを見れば、このクラスが正しいと確認できる」状態を作るのがカプセル化の目的。

### `Account` の場合

`00/ex02/Account.hpp`

```cpp
	static int	_nbAccounts;
	static int	_totalAmount;
	static int	_totalNbDeposits;
	static int	_totalNbWithdrawals;

	static void	_displayTimestamp( void );

	int				_accountIndex;
	int				_amount;
	int				_nbDeposits;
	int				_nbWithdrawals;
```

`Account` の不変条件はもっと強力。

- **`_totalAmount` は、全ての `Account` の `_amount` の合計と等しい**
- `_nbAccounts` は、生きている `Account` の個数と等しい

これは **1つのオブジェクトの中で完結しない不変条件**。誰かが `_amount` だけを直接書き換えると、即座に `_totalAmount` との整合性が崩れる。だから `makeDeposit()` の中で両方を同時に更新する必要があり、その手続きを飛ばせないように `private` にする。

```cpp
void Account::makeDeposit(int deposit) {
    _amount += deposit;        // 個別残高
    _totalAmount += deposit;   // 合計残高 ← これを忘れると不変条件が壊れる
    _nbDeposits++;
    _totalNbDeposits++;
}
```

**この「必ずセットで行わなければならない操作」を1つの関数にまとめ、外から個別に触れないようにすることが、クラスの中心的な役割。**

### まとめ

| | データが public | データが private |
|---|---|---|
| 不変条件を壊せる場所 | プログラム全体 | そのクラスの実装ファイルだけ |
| バグ調査の範囲 | 全コード | 数十〜数百行 |
| 「必ずセットの操作」 | 呼び出し側が守る（守り忘れる） | クラスが保証する |

---

## 目的2: 不変条件の確立と後片付けを自動化する（コンストラクタ・デストラクタ・RAII）

これは **C++ において最も重要** で、しかも **クラスでなければ絶対に実現できない** 機能。

### コンストラクタ: 不正な状態の瞬間を作らない

`struct` と関数だけで同じことをやろうとすると、こうなる。

```cpp
// クラスを使わない場合
struct PhoneBook {
    Contact contacts[8];
    int contactCount;
    int oldestContactIndex;
};

void initPhoneBook(PhoneBook* pb) {
    pb->contactCount = 0;
    pb->oldestContactIndex = 0;
}

// 使う側
PhoneBook book;        // ← この瞬間、contactCount は不定値（ゴミ）
                       //   初期化を忘れたら、そのまま壊れたオブジェクトが使われる
initPhoneBook(&book);  // 呼ぶのを忘れる可能性がある
```

`PhoneBook book;` から `initPhoneBook(&book);` までの間、**オブジェクトは存在するが不正な状態**。そして初期化関数を呼ぶのは呼び出し側の責任なので、**忘れられる**。

コンストラクタがあるとこうなる。

```cpp
PhoneBook book;   // コンストラクタが自動で呼ばれる
                  // この行が終わった時点で不変条件は必ず成立している
```

**「オブジェクトが存在するのに不正な状態」という瞬間が原理的に存在しなくなる。** 初期化を忘れることが不可能になる。

これは ex03 の `HumanB` で悩んだ点そのもの。

```cpp
HumanB::HumanB(std::string const &name) : _name(name), _weapon(NULL) {}
//                                                     ^^^^^^^^^^^^^
// 「_weapon は NULL か、有効な Weapon を指す」という不変条件を
// コンストラクタが確立する。これがないと不定値（何を指すか分からない）
```

初期化リストで `NULL` を入れることは、**不変条件を確立する作業**。これを書かないと「`_weapon` は NULL か有効なポインタのいずれか」という前提が成立せず、`attack()` の `if (_weapon)` チェックが意味を失う。

### デストラクタ: 後片付けを忘れられなくする

デストラクタは、スコープを抜けるときに **必ず** 呼ばれる。途中で `return` しても、例外が飛んでも呼ばれる。

```cpp
void f() {
    Zombie z("foo");   // コンストラクタ
    if (something)
        return;        // ← ここで抜けてもデストラクタは呼ばれる
    // ...
}                      // ← ここでも呼ばれる
```

関数だけで書くと、`return` の手前ごとに後片付けを書く必要があり、1箇所忘れるとリーク・バグになる。

### RAII

**RAII（Resource Acquisition Is Initialization、リソース取得は初期化である）** … リソース（メモリ、ファイル、ロックなど）の取得をコンストラクタで行い、解放をデストラクタで行う設計手法。

ex04 で使う `std::ifstream` がまさにこれ。

```cpp
{
    std::ifstream in("file.txt");   // コンストラクタでファイルを開く
    // ... 使う ...
    // in.close() を書かなくてもよい
}   // デストラクタが自動で閉じる。途中で return しても閉じる
```

C 言語ならこうなる。

```c
FILE* f = fopen("file.txt", "r");
if (!f) return;
/* ... */
if (error) {
    fclose(f);      /* ← 各 return の前に必ず書く */
    return;
}
fclose(f);          /* ← 忘れるとリソースリーク */
```

**RAII は C++ が他の言語に対して持つ最大の武器であり、コンストラクタとデストラクタ、つまりクラスがなければ成立しない。** 自由関数ではどうやっても実現できない。

Module 01 の ex00 で `new` / `delete` の対応を手で管理して苦労した部分は、Module 02 以降でこの仕組みに置き換わっていく。

---

## 目的3: 実装を隠して変更の影響範囲を閉じ込める（情報隠蔽）

### 用語

**情報隠蔽（information hiding）** … モジュールの内部の作り方を外から見えなくすること。カプセル化と近いが、狙いは **不変条件の保護ではなく、変更への耐性**。

David Parnas が 1972 年の論文 *"On the Criteria To Be Used in Decomposing Systems into Modules"* で示した基準。

> モジュールの分け方の基準は「処理の手順」ではなく、
> **「将来変わりそうな決定を、それぞれ1つのモジュールの中に隠す」** ことである。

### `Weapon` で見る

`01/ex03/Weapon.hpp`

```cpp
class Weapon {
    private:
        std::string _type;
    public:
        Weapon(std::string const &type);
        ~Weapon(void);
        std::string const &getType(void) const;
        void setType(std::string const &type);
};
```

外から見えているのは `getType()` と `setType()` の2つだけ。「中身が `std::string` である」というのは実装の詳細。

仮に「メモリを節約したいので `char _type[32]` に変える」としたとき。

- `_type` が `private` なら … 変更が必要なのは `Weapon.hpp` と `Weapon.cpp` だけ。`HumanA.cpp`、`HumanB.cpp`、`main.cpp` は **1行も変わらない**（`getType()` が `const std::string&` を返し続ける限り）
- `_type` が `public` で、各所で `weapon._type` と直接触っていたら … `_type` を使っている **すべての箇所** を書き換える必要がある

**public にしたメンバーは、後から変えられなくなる。** `public` は「これは今後も変えないと約束する」という宣言。

### 公開したものは減らせない

実務で最も痛い点。一度 `public` にしたものは、他のコードが依存するため、変更・削除が困難になる。だから **最初は最小限だけ公開する** のが原則。

これは Module 00 の `Account` で `Account(void)` が `private` に置かれていた理由でもある。

```cpp
private:
	// ...
	Account( void );   // デフォルトコンストラクタを private に
```

「初期入金額なしで口座を作らせない」という設計判断を、**コンパイラに強制させている**。
コメントで「使わないでください」と書くのとは根本的に違う。

---

## 目的4: 概念に型を与えて間違いをコンパイラに検出させる（抽象データ型）

### 用語

**抽象データ型（Abstract Data Type, ADT）** … データの内部表現を隠し、「そのデータに対して何ができるか」という操作の集合だけで定義される型。Barbara Liskov と Stephen Zilles が 1974 年に定式化した。

### 型が違うと、間違いがコンパイルエラーになる

`Weapon` は中身が `std::string` 1個。「なら `std::string` をそのまま使えばいいのでは」と思えるが違う。

```cpp
// Weapon 型がある場合
void HumanB::setWeapon(Weapon &weapon);

HumanB jim("Jim");
jim.setWeapon(someName);      // コンパイルエラー: std::string は Weapon ではない
```

```cpp
// std::string で済ませた場合
void setWeapon(std::string &weaponType);

jim.setWeapon(playerName);    // コンパイル通る。名前を武器として設定してしまう
```

**同じ「文字列1個」でも、名前と武器種別は違う概念。** それを別の型にすることで、混同がコンパイル時に発見される。

これは「**不正な状態を表現できないようにする**（making illegal states unrepresentable）」という考え方。実行時にチェックして落とすのではなく、**そもそも書けなくする** ほうが強い。

### 型は意味の伝達手段

```cpp
void attack(Weapon& w);                 // 何を渡すべきか明白
void attack(std::string& s);            // 何の文字列? 名前? 武器? メッセージ?
void attack(std::string&, int, bool);   // 完全に不明
```

型名は、関数の使い方をドキュメントなしに伝える。しかもコメントと違って **コンパイラが強制する** ので、嘘にならない。

---

## 目的5: 呼び出し側を変えずに振る舞いを差し替える（多態性）

Module 03・04 で本格的に扱う内容。歴史的には **OOP の中心目的**。

### 用語

**多態性（polymorphism）** … 同じ呼び出し方で、対象の実際の型に応じて違う処理が実行されること。

**仮想関数（virtual function）** … 派生クラスで上書きでき、実行時に実際の型に応じた版が呼ばれる関数。

### 何が嬉しいのか

多態性がない場合、種類が増えるたびに **呼び出し側** を書き換える。

```cpp
// 分岐で書く場合
void makeSound(Animal& a) {
    if (a.type == DOG)
        std::cout << "Woof" << std::endl;
    else if (a.type == CAT)
        std::cout << "Meow" << std::endl;
    // 動物を追加するたびに、この関数を修正する
    // 同じような if 連鎖が、プログラム中の何十箇所にもある
}
```

多態性を使う場合、呼び出し側は変わらない。

```cpp
class Animal {
public:
    virtual void makeSound() const = 0;   // 純粋仮想関数
};

class Dog : public Animal {
public:
    void makeSound() const { std::cout << "Woof" << std::endl; }
};

// 呼び出し側
void f(Animal& a) {
    a.makeSound();   // Dog なら Woof、Cat なら Meow。この行は永久に変わらない
}
```

新しい動物を追加するとき、**既存のコードを1行も触らずに** 新しいクラスを足すだけで済む。
これを **開放閉鎖原則（Open-Closed Principle）** — 拡張に対して開いており、修正に対して閉じている — と呼ぶ。

### 歴史的な位置づけ

この機能は **Simula 67**（Ole-Johan Dahl と Kristen Nygaard、ノルウェー）が最初に持ち込んだ。`class`、`subclass`、`virtual` という語も Simula 由来。

Stroustrup は Simula でシミュレーションのプログラムを書いた経験から C++ を作った。HOPL-2 §3.3 の「C with Classes から C++ への6つの追加」の第1項が **`[1] Virtual functions.`** だったのは、これが最重要だったから。

つまり **Stroustrup にとって「クラス」の完成形は多態性を伴うもの** だった。Module 01 で扱っているクラスは、まだその手前の段階。

---

## 目的6: 状態と操作をまとめる（凝集）

**凝集（cohesion）** … 関連するものが同じ場所にまとまっている度合い。

一般に最も多く語られる理由だが、**C++ では最も弱い理由**。C++ には名前空間と自由関数があるので、まとめるだけならクラスは要らない。

```cpp
// namespace でまとめる
namespace text {
    std::string replaceAll(const std::string&, const std::string&, const std::string&);
    std::string trim(const std::string&);
}

text::replaceAll(content, "a", "b");
```

Java には自由関数がないので、関数を置くために `Collections`、`Arrays` のような **static メソッドだけのクラス** を作らざるを得ない。あれは言語制約への対処であり、設計として望ましいからではない。

C++ でこれを真似すると、**構築する意味のないオブジェクト** が生まれる。

---

## よくある誤解

「クラスの目的」として広く語られているが正確でないもの。

### 誤解1「再利用性のためである」

入門書で最も頻繁に見る説明だが、実態と合っていない。

- **継承による再利用は脆い** … 親クラスを変えると全ての子が壊れる（**脆い基底クラス問題**, fragile base class problem）
- そのため現代の指針は **「継承より合成（composition over inheritance）」**。GoF『デザインパターン』（1994）が明示している
- 再利用しやすいのは、むしろ **依存の少ない自由関数**。`std::sort` はどんな型にも使える

再利用は結果として得られることもあるが、**目的ではない**。目的は 1〜5、特に「不変条件の保護」と「変更の局所化」。

### 誤解2「現実世界をそのままモデル化するためである」

Simula がシミュレーション用言語だった名残だが、誤解を生む。プログラムの多くは現実の模写ではない。`std::string` や `std::ifstream` に対応する「現実の物」はない。

`Weapon` クラスの価値は「現実の武器に対応しているから」ではなく、**型として区別され、`_type` の不変条件が守られるから**。

### 誤解3「すべてをクラスにすべきである」

Stroustrup 自身が繰り返し否定している。C++ は **手続き型・データ抽象・オブジェクト指向・ジェネリックプログラミング** の複数のスタイルを支援する言語であり、OOP 専用言語ではない。

さらに、OOP の命名者である Alan Kay は、OOP の本質は **メッセージ送信** であってクラスではないと述べている。「クラス = OOP」という等式自体が後付け。

---

## 判断基準

### 単一の判断基準

> **そのクラスから `private` メンバーを取り除いたとき、何か失われるか。**

失われないなら、それはクラスである必要のない関数。

### 詳細なチェックリスト

| 問い | Yes なら |
|---|---|
| 呼び出しをまたいで保持する状態があるか | クラス寄り |
| 複数のデータの間に、常に保つべき関係（不変条件）があるか | **クラス（最重要）** |
| 「必ずセットで行うべき操作」があるか | クラス |
| 生成時に必ず初期化が必要か | クラス（コンストラクタ） |
| 終了時に必ず解放が必要なリソースを持つか | **クラス（RAII）** |
| 内部表現を将来変えたいか | クラス（情報隠蔽） |
| 他の似た型と混同されると危険か | クラス（型による区別） |
| 振る舞いを実行時に差し替えたいか | クラス（多態性） |
| 単に関数をまとめたいだけか | **namespace で十分** |

### 自分のコードに当てはめる

| 対象 | 該当する目的 | 判定 |
|---|---|---|
| `PhoneBook` | 不変条件（count と配列の整合）、初期化必須 | クラスが妥当 |
| `Account` | 不変条件（`_totalAmount` = 全 `_amount` の和）、生成／破棄で静的カウンタ更新 | クラスが妥当 |
| `Weapon` | 型による区別、内部表現の隠蔽、`setType` で状態変更 | クラスが妥当 |
| `HumanB` | `_weapon` が NULL か有効という不変条件、`_name` の保持 | クラスが妥当 |
| **ex04 の置換処理** | 状態なし、不変条件なし、リソースなし、差し替え不要 | **自由関数** |

これは C++ Core Guidelines の **C.4「クラスの内部表現に直接アクセスする必要がある場合にのみ、メンバ関数にする」**、および Effective C++ **Item 23「メンバ関数よりも非メンバ非フレンド関数を選べ」** が示す判断と一致する。

---

## 42 のカリキュラムとの対応

Module ごとに、上の6つのどれを学んでいるか。

| Module | 主題 | 対応する目的 |
|---|---|---|
| **00** | クラスの基本、`private`、static メンバー | 1（カプセル化）、3（情報隠蔽） |
| **01** | メモリ、`new`/`delete`、ポインタと参照 | 2 の準備（寿命の理解） |
| **02** | 演算子オーバーロード、Orthodox Canonical Form（コピーコンストラクタ・代入演算子・デストラクタ） | **2（RAII、コピー意味論）**、4（ADT） |
| **03** | 継承 | 5 の準備 |
| **04** | 抽象クラス、インターフェース、多態性 | **5（多態性）** |

**Module 02 で RAII とコピー制御、Module 04 で多態性** に到達する。Module 01 は、その土台としてオブジェクトの寿命と指し方を固めている段階。

ex04 の `std::ifstream` は、Module 02 で自分で書くことになる RAII の **完成品を使う側から体験する** 位置づけ。

---

## ex04 での結論

### 課題要件の確認

課題文（subject）が ex04 で指定しているのは4つだけ。

1. **引数** … filename, s1, s2 の3つをこの順で受け取る
2. **動作** … `<filename>` を開き、s1 を s2 に置換しながら `<filename>.replace` にコピー
3. **禁止** … `std::string::replace`、C のファイル操作関数
4. **提出ファイル** … `Makefile, main.cpp, *.cpp, *.{h, hpp}`

**クラスにせよとも、関数にせよとも書かれていない。** 実装の形は自由。

`*.cpp, *.{h, hpp}` はファイル追加の許可であって、クラスの要求ではない。
ex03 はクラスを要求していたので `Weapon.{h, hpp}`, `Weapon.cpp` のように **ファイル名まで列挙されていた**。ex04 がワイルドカードなのは「構成は任せる」という意味。

### 課題文と評価シートの違い

| 文書 | 位置づけ | ex04 の記述 |
|---|---|---|
| **課題文（subject PDF）** | 実装すべき仕様 | 動作と禁止事項のみ。実装形態の指定なし |
| **評価シート（intra）** | 評価者が defense で確認する項目 | "There is a **function** replace (or other name) that works as specified in the subject" |

評価シートも公式文書で defense で読まれるが、「as specified in **the subject**」と課題文に差し戻しており、シート自身が仕様を定めているわけではない。「or other name」も形と名前に幅を認めている表れ。

### 設計上の判断

置換処理は 6 つの目的のどれも必要としない。

```cpp
std::string replaceAll(const std::string& content,
                       const std::string& s1,
                       const std::string& s2);
```

- 呼び出しをまたいで保持する状態がない
- 同じ入力なら常に同じ出力（副作用も履歴もない）
- **保護すべき不変条件が存在しない**
- 解放が必要なリソースを持たない
- 振る舞いを差し替える必要がない

守るものがないので、カプセル化しても得るものがない。`private` メンバーが0個のクラスは、カプセル化していない。

さらに、自由関数のほうが呼び出し側の制約が少ない。

```cpp
// 自由関数: そのまま呼べる
std::string r = replaceAll(content, s1, s2);

// クラス: 使う前にオブジェクトを作らされる
Replacer rep;
std::string r = rep.replace(content, s1, s2);   // rep は何のために存在するのか
```

`rep` は何も保持していないので、構築する手間だけが増えている。

### 結論

**ex04 は自由関数で実装する。** ファイルを分けたい場合も、クラスにするのではなく「関数の宣言を `.hpp`、定義を `.cpp`」に分ける。

```
01/ex04/
├── Makefile
├── main.cpp      // argc/argv チェック、ファイル I/O
├── replace.hpp   // 関数宣言
└── replace.cpp   // 関数定義
```

「`.hpp` に宣言、`.cpp` に定義」という Module 00 で学んだ分割は、**クラスに限らず自由関数にもそのまま適用できる**。

なお、名前を `replace` にするのは問題ない。禁止されているのは `std::string::replace`（メンバ関数）であって、自作の自由関数 `replace` は別物。

逆にこの課題で `std::ifstream` / `std::ofstream` を使うことは、**目的2（RAII）の恩恵を利用者として体験する** ことにあたる。`close()` を書かなくてもファイルが閉じられる理由が、デストラクタ。

---

## 参考資料

- David Parnas, *"On the Criteria To Be Used in Decomposing Systems into Modules"*, 1972 — 情報隠蔽の原典
- Liskov & Zilles, *"Programming with Abstract Data Types"*, 1974 — 抽象データ型の定式化
- Bjarne Stroustrup, *"A History of C++: 1979–1991"*（HOPL-2）— https://stroustrup.com/hopl2.pdf §3.3 に C++ への6つの追加
- Scott Meyers, *Effective C++ 3rd Edition* — Item 22（データメンバーは private に）、Item 23（非メンバ非フレンド関数を選べ）
- C++ Core Guidelines — https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines C.1, C.2, C.4
- Gamma et al., *Design Patterns*, 1994 — 「継承より合成」
