# CPP Module 01 — 何を学んだか

後日この課題を見返したときに、文法の要点と当時考えたことを思い出すためのメモ。
課題の手順書ではない。

各演習の詳細は `ex00/README.md` 〜 `ex02/README.md` に分けてある。ここは全体の索引で、ex03 の内容は本ファイルにまとめてある。

---

## 目次

1. [ディレクトリ構成](#1-ディレクトリ構成)
2. [課題ごとの到達点](#2-課題ごとの到達点)
3. [スタックとヒープ](#3-スタックとヒープ)
4. [new / delete と new[] / delete[]](#4-new--delete-と-new--delete)
5. [ポインタ](#5-ポインタ)
6. [参照](#6-参照)
7. [ポインタと参照の使い分け](#7-ポインタと参照の使い分け)
8. [参照が再束縛できない理由](#8-参照が再束縛できない理由)
9. [const が置かれる3つの位置](#9-const-が置かれる3つの位置)
10. [値渡し・参照渡し・戻り値](#10-値渡し参照渡し戻り値)
11. [クラスとファイル分割の再確認](#11-クラスとファイル分割の再確認)
12. [Makefile](#12-makefile)
13. [評価で聞かれやすいこと](#13-評価で聞かれやすいこと)
14. [Module 02 に向けて](#14-module-02-に向けて)
15. [課題中に悩んだこと](#15-課題中に悩んだこと)
16. [参考資料](#16-参考資料)

---

## 1. ディレクトリ構成

```
01/
├── ex00/   BraiiiiiiinnnzzzZ     — スタックとヒープ、new / delete
├── ex01/   MoarBrainz            — new[] / delete[]、デフォルトコンストラクタ
├── ex02/   HI_THIS_IS_BRAIN      — ポインタと参照のアドレス・値
├── ex03/   HUMAN_A_AND_B         — 参照メンバーとポインタメンバーの使い分け
└── README.md
```

Module 01 全体のテーマは **メモリとその指し方**。Module 00 がクラスの作り方なら、01 は「オブジェクトがどこに置かれ、どう指されるか」。

---

## 2. 課題ごとの到達点

| 課題 | 作ったもの | 学んだこと |
|------|------------|------------|
| **ex00** | ヒープとスタックに Zombie を1体ずつ作る | `new` / `delete`、スコープと寿命、デストラクタのタイミング |
| **ex01** | N 体の Zombie を1回の確保で作る | `new[]` / `delete[]`、配列がデフォルトコンストラクタを要求する理由 |
| **ex02** | 1つの文字列をポインタと参照で見る | アドレスと値、`*` と `&`、参照は別名であること |
| **ex03** | Weapon を参照で持つ HumanA、ポインタで持つ HumanB | 参照メンバーとポインタメンバーの設計判断 |

ex00 から ex03 まで、**同じオブジェクトを別の経路で指す**という一つの話が段階的に深くなっている。

---

## 3. スタックとヒープ

### 用語

- **スタック** … 関数のローカル変数が置かれる領域。スコープ `{ }` を抜けると自動で破棄される
- **ヒープ** … `new` で確保する領域。明示的に `delete` するまで残る

### 文法

```cpp
void randomChump(std::string name) {
    Zombie zombie(name);   // スタック
    zombie.announce();
}   // ここでデストラクタが自動で呼ばれる

Zombie* newZombie(std::string name) {
    return new Zombie(name);   // ヒープ。関数を抜けても残る
}   // 呼び出し側が delete する責任を負う
```

| | スタック | ヒープ |
|--|----------|--------|
| 破棄 | スコープ終了時に自動 | `delete` を書いたときだけ |
| 関数の外へ返す | できない | ポインタで返せる |
| 忘れたときの結果 | 問題なし | メモリリーク |

### 複合文（ブロックスコープ）

**複合文（compound statement）** … `{` と `}` で囲んだ文の並び。C++ では **新しいスコープ** を作る。

`if` / `for` / `while` / 関数本体に付いている `{}` も複合文だが、**制御構文がなくても単独で書ける**。Python にはこの書き方はない（スコープは `def` や `class` などで決まる）。C / Go も同様に単独の `{}` が使える。

```cpp
int main() {
    {   // ブロック1
        Weapon club = Weapon("crude spiked club");
        HumanA bob("Bob", club);
        bob.attack();
    }   // bob → club の順にデストラクタが呼ばれ、変数は消える

    {   // ブロック2（別スコープなので club を再宣言できる）
        Weapon club = Weapon("crude spiked club");
        HumanB jim("Jim");
        jim.setWeapon(club);
        jim.attack();
    }

    return 0;
}
```

| 効果 | 内容 |
|------|------|
| スコープ | 中で宣言した変数は `{}` の中だけ有効 |
| 破棄 | ブロックを抜けるとスタック上のオブジェクトが **宣言の逆順** で破棄される |
| 名前の再利用 | 別ブロックなら同じ変数名（例: `club`）を再宣言できる |
| ex03 での意図 | HumanA 用と HumanB 用のテストを分離する。`club` を `bob` より先に宣言し、`bob` 破棄後に `club` が破棄される順序にする（§7 参照） |

`{}` は Weapon 専用の記法ではなく、**ローカル変数の寿命と可視範囲を区切る** C++ の基本構文。ex03 の `main` が `{}` だけのブロックを2つ並べているのは、課題のテストコードとしてよくある書き方。

### 考えたこと

`newZombie` と `randomChump` はメンバー関数ではない。まだ存在しないオブジェクトを作る関数なので、クラスの外に置いた。

工場関数を別の `.cpp` に分けても、`main.cpp` から呼ぶには宣言が必要になる。コンパイラは `.cpp` を1つずつ処理するので、`main.cpp` をコンパイルしている間は `newZombie.cpp` の中身を知らない。

---

## 4. new / delete と new[] / delete[]

### 文法

```cpp
Zombie* z     = new Zombie(name);    delete z;
Zombie* horde = new Zombie[N];       delete[] horde;
```

対応するものを組にして使う。`new[]` したものを `delete` で解放すると未定義動作になる。`new[]` は要素数を記録しており、`delete[]` はそれを読んで N 回デストラクタを呼んでから領域を解放する。`delete` を使うと1体分しか破棄されない。

### `new[]` がデフォルトコンストラクタを要求する理由

```cpp
Zombie* horde = new Zombie[N];
```

この行は **引数なしのコンストラクタを N 回**呼ぶ。C++98 には配列全体に引数を渡す構文がない。そのため、空の状態で N 個作ってから `setName` で名前を入れる形になる。

```cpp
Zombie* zombieHorde(int N, std::string name) {
    Zombie* horde = new Zombie[N];
    for (int i = 0; i < N; i++)
        horde[i].setName(name);
    return horde;
}
```

### 考えたこと

setter を用意したのは「Zombie がエンティティだから」ではなく、**`new[]` がデフォルトコンストラクタを必要とするから**。ex00 ではコンストラクタで名前を渡していた。

Module 00 の `PhoneBook` で `Contact _contacts[8]` がデフォルトコンストラクタを必要としたのと同じ理由。配列は要素を引数なしで構築する。

---

## 5. ポインタ

### 用語

**ポインタ** … アドレスを値として保持する変数。それ自身がメモリを占める。

### 文法

```cpp
Weapon club("crude spiked club");
Weapon* p = &club;

p              // 指す先のアドレス
*p             // 指す先のオブジェクト
p->getType()   // 指す先のメンバー呼び出し。(*p).getType() と同じ
&p             // ポインタ変数 p 自身のアドレス（別の番地）
p = &sword;    // 指す先を変更できる
p = NULL;      // 「対象なし」を表せる
```

### ポインタが持つ3つの状態

| 状態 | 意味 |
|------|------|
| 未初期化（不定値） | 読むと未定義動作。**言語は防いでくれない** |
| `NULL` | 対象がないことを表す |
| 有効なアドレス | 対象を指している |

1番目を潰すのはプログラマの責任。C++98 では `NULL` を使う（`nullptr` は C++11）。

### 考えたこと

`HumanB` で `_weapon` を初期化リストに書き忘れた。コンパイルは通ってしまい、`if (!_weapon)` が不定値を読む状態になっていた。

`std::string _name` が書かなくても空文字列になるのは、クラス型のメンバーにはデフォルトコンストラクタが自動で走るから。ポインタや `int` は組み込み型なのでコンストラクタがなく、何も起きない。

---

## 6. 参照

### 用語

**参照** … 既存オブジェクトの別名。変数ではなく、それ自身のメモリを持たない。

### 文法

```cpp
Weapon club("crude spiked club");
Weapon& r = club;      // 初期化必須

r              // club そのもの
r.getType()    // club.getType() と同じ
&r             // &club と同じ
r = sword;     // club = sword という意味。付け替えではない
```

参照には「自分自身」を指す構文がない。`r` と書いた式は常に `club` を意味する。

### ex02 で確認したこと

```cpp
std::string const str = "HI THIS IS BRAIN";
std::string* stringPTR = &str;
std::string& stringREF = str;
```

| 式 | 出力 |
|----|------|
| `&str` / `stringPTR` / `&stringREF` | すべて同じアドレス |
| `str` / `*stringPTR` / `stringREF` | すべて同じ値 |

3つのアドレスが一致するのは、参照が独立した変数を持たないため。`&stringPTR` だけは別の番地になる（ポインタ変数自身の場所）。

---

## 7. ポインタと参照の使い分け

判断軸は2つあり、独立している。

| | 参照 `T&` | ポインタ `T*` |
|--|-----------|---------------|
| 「対象なし」を表せるか | 表せない | `NULL` で表せる |
| 途中で付け替えられるか | できない | できる |
| 未初期化を防ぐのは | **コンパイラ** | プログラマ |
| アクセス | `.` | `->` |
| `sizeof` | 対象のサイズ | ポインタのサイズ |

参照が使えるのは「対象が必ず存在し、途中で変わらない」場合だけ。それ以外は全部ポインタになる。

### ex03 での適用

| | HumanA | HumanB |
|--|--------|--------|
| メンバー | `Weapon &_weapon` | `Weapon *_weapon` |
| 初期化 | 初期化リストで必須 | `NULL` にしてから `setWeapon` |
| 武器なしの状態 | 表現できない | 表現できる |
| アクセス | `_weapon.getType()` | `_weapon->getType()` |

`HumanA` に参照を使うと、「常に武装している」という仕様がコンパイラによって強制される。引数なしのコンストラクタを作れず、初期化を忘れるとビルドが失敗し、`attack()` に NULL チェックが不要になる。

`setWeapon` は参照で受け取ってアドレスを保存する。

```cpp
void HumanB::setWeapon(Weapon &weapon) {
    this->_weapon = &weapon;   // main が jim.setWeapon(club) と書くため引数は参照
}
```

### 参照メンバーの代償

参照メンバーを持つクラスは代入できない。参照は付け替えられないので、代入の意味を定義できないため。C++98 の `std::vector` は要素が代入可能であることを要求するので、`std::vector<HumanA>` も使えない。参照メンバーが実際のコードで多くないのはこのため。

### 保証されないこと

参照は NULL にならないが、**参照先が生きていることまでは保証しない**。`HumanA` は `club` が自分より長く生きることが前提になる。ex03 の `main` では両方が同じ複合文の中にあり、`club` を先に宣言しているので `bob` が先に破棄され、そのあと `club` が破棄される（§3 複合文）。

---

## 8. 参照が再束縛できない理由

「再代入できない」という表現は不正確。正確には **再束縛（rebinding）ができない**。値の代入はできる。

```cpp
int a = 5, b = 7;
int& r = a;

r = 100;   // できる。a が 100 になる
r = b;     // a = b という意味。a が 7 になる。r は依然 a の別名
```

`=` の意味が「束縛先への代入」に割り当てられているため、「束縛先を変える」を書く構文が残っていない。

### 設計者による説明

Stroustrup 本人が HOPL-2 論文 §3.3.4 に理由を書いている。

> References were introduced primarily to support operator overloading.
>
> Problems with debugging Algol68 convinced me that having references that didn't change what object they referred to after initialization was a good thing. **If you wanted to do more complicated pointer manipulation in C++ you can use pointers.** Because C++ has both pointers and references it does not need operations for distinguishing operations on the reference itself from operations on the object referred to (like Simula) or the kind of deductive mechanism employed by Algol68.

要点は3つ。

1. 参照は**演算子オーバーロードのために**導入された
2. Algol68 のデバッグ経験から、初期化後に対象が変わらない方がよいと判断した
3. 複雑な操作が必要ならポインタを使えばよい。両方あるので、参照自身への操作と参照先への操作を区別する演算子を用意する必要がない

同じ節に、参照を返す `operator[]` の例が載っている。

```cpp
class String {
    char& operator[](int index);   // 参照を返す
};
s[i] = c1;   // 代入できる。= が再束縛だとこれが成立しない
```

**ポインタが先にあり、参照は後から特定の目的のために追加された**（C は 1972 年、参照は C with Classes から C++ への移行期に追加）。だから「ポインタでできることを参照に足す」動機がなかった。

C++11 では `std::reference_wrapper` が「再束縛できる参照」としてライブラリに追加された。言語機能としてではなく、別の道具として分けられている。

---

## 9. const が置かれる3つの位置

```cpp
std::string const &getType(void) const;
            ^^^^^                 ^^^^^
             (1)                   (2)
```

| 位置 | 意味 |
|------|------|
| 型に付く `const`（1） | その参照・ポインタ経由で対象を書き換えられない |
| 関数末尾の `const`（2） | この関数はメンバーを変更しない。`this` が const になる |
| メンバー変数に付く `const` | そのオブジェクトの寿命中、値を変えられない |

`getType()` が `const std::string&` を返すのは、**コピーを作らず、かつ外から `_type` を書き換えられないようにする**ため。`const` を外すと次が通ってしまう。

```cpp
club.getType() = "何でも";   // setType を用意した意味がなくなる
```

`const` は書き込みの可否だけを決める。コピーの有無は「値か参照か」が決める。この2つは独立している。

---

## 10. 値渡し・参照渡し・戻り値

### 引数

| 書き方 | リテラルを渡せる | 引数のコピー | 意味 |
|--------|------------------|--------------|------|
| `std::string type` | できる | 発生する | 値を受け取る |
| `std::string& type` | **できない** | 発生しない | 呼び出し側を書き換える |
| `std::string const& type` | できる | 発生しない | 読むだけ |

一時オブジェクトは非 const 参照に束縛できない。`club.setType("some other type of club")` は一時的な `std::string` を作るので、引数が `std::string&` だとコンパイルエラーになる。

```
error: invalid initialization of non-const reference of type 'std::string&' from an rvalue
```

非 const 参照は「呼び出し側を書き換える」という宣言だが、一時オブジェクトを書き換えても誰も結果を読めないため禁止されている。

### 戻り値

| 書き方 | コピー |
|--------|--------|
| `std::string getType() const` | **呼ぶたびに毎回**作られる |
| `const std::string& getType() const` | 作られない |

参照で返せるのは、返す対象が関数を抜けたあとも生きている場合だけ。`_type` はメンバー変数なので安全。ローカル変数の参照を返すと、破棄済みの領域を指すことになる。

### 引数が参照でも、メンバーはコピーされる

```cpp
void Weapon::setType(std::string const &type) {
    this->_type = type;   // コピー代入。_type は自分のデータを持つ
}
```

`_type` は `std::string`（値メンバー）なので、呼び出し側の文字列をあとから変更しても `_type` は変わらない。引数の参照は関数の実行中しか存在しない。

「引数が参照であること」と「メンバーが参照であること」は別。後者が `HumanA` の `Weapon& _weapon` で、こちらは元のオブジェクトを見続ける。

---

## 11. クラスとファイル分割の再確認

Module 00 で扱った内容だが、ex03 で繰り返し引っかかった点。

**ヘッダガードのマクロ名はファイルごとに固有にする。** `HumanA.hpp` に `WEAPON_HPP` を使うと、`Weapon.hpp` の中身が丸ごとスキップされ、`Weapon` が未宣言になる。

```
HumanA.hpp が WEAPON_HPP を定義
  → Weapon.hpp の #ifndef WEAPON_HPP が偽になる
  → class Weapon が読み込まれない
  → error: 'Weapon' does not name a type
```

**`.hpp` は宣言、`.cpp` は定義。** `.hpp` に `{}` まで書くと定義になり、`.cpp` にも定義があると二重定義になる。

**宣言と定義は引数の型まで一致させる。** 片方だけ `const std::string&` に変えると、別の関数として扱われる。

```
error: prototype for 'HumanB::HumanB(const std::string&)' does not match any in class 'HumanB'
```

**`<iostream>` は使う `.cpp` に include する。** `attack()` の実装だけが `std::cout` を使うので、`.hpp` には不要。

---

## 12. Makefile

```makefile
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = HUMAN_A_AND_B
SRCS = main.cpp HumanA.cpp HumanB.cpp Weapon.cpp
OBJS = $(SRCS:.cpp=.o)
```

演習をコピーして始めると `NAME` と `SRCS` が前の演習のままになりやすい。`.cpp` を追加したら `SRCS` に足す。

ヒープを使う ex00 / ex01 は valgrind で確認できる。

```bash
valgrind --leak-check=full ./MoarBrainz
```

`All heap blocks were freed -- no leaks are possible` が出れば `delete` 漏れなし。

---

## 13. 評価で聞かれやすいこと

| トピック | 答えの要点 |
|----------|------------|
| スタックとヒープの違い | 自動破棄か、`delete` が必要か。関数の外へ返せるか |
| `new` と `new[]` | 単体か配列か。`delete` / `delete[]` と組にする |
| 混ぜるとどうなるか | 未定義動作。`delete` は1要素分しか破棄しない |
| なぜ `new[]` にデフォルトコンストラクタが必要か | 引数なしで N 回構築するため。C++98 に配列へ引数を渡す構文がない |
| ポインタと参照の違い | NULL の可否、再束縛の可否、未初期化を防ぐのが誰か |
| 参照は再代入できるか | 値の代入はできる。再束縛はできない |
| なぜ再束縛できないか | `=` が代入に割り当てられており、束縛先を変える構文がない |
| `stringPTR` と `&stringPTR` | 指す先のアドレスと、ポインタ変数自身のアドレス |
| なぜ `const T&` で受け取るか | コピーを避け、変更しないことを示す。一時オブジェクトも渡せる |
| `getType()` が const 参照を返す理由 | コピーを作らず、外から private メンバーを書き換えさせない |
| ex03 でポインタと参照をどう使い分けたか | 常に存在するなら参照、存在しないことがあるならポインタ |
| 複合文（単独の `{}`）とは | 新しいスコープを作る。if/for なしでも書ける。抜けるとローカル変数が逆順で破棄される |

---

## 14. Module 02 に向けて

| トピック | Module 01 とのつながり |
|----------|------------------------|
| Orthodox Canonical Form | コピーコンストラクタ、コピー代入演算子、デストラクタ。参照メンバーがあると代入が定義できない件の続き |
| 演算子オーバーロード | 参照が導入された本来の目的。`operator[]` や `operator=` が参照を返す理由 |
| 固定小数点数 | `const` メンバー関数、`const` 参照引数をそのまま使う |
| 浅いコピーと深いコピー | ポインタメンバーを持つクラスをコピーしたときの問題 |

---

## 15. 課題中に悩んだこと

詳細は上の各節と、ex00 〜 ex02 の README。

| 悩み | 結論 |
|------|------|
| なぜ工場関数の宣言を `main.cpp` に書くのか | コンパイラは `.cpp` を1つずつ処理する。実装を分けても宣言は自動では見えない |
| Entity だから setter を使うのか | 違う。`new[]` がデフォルトコンストラクタを要求するから |
| 参照は初回だけコピーを作るのか | 一度も作らない。値で返す場合は毎回作られる |
| コピーを決めるのは `const` か | 違う。「値か参照か」が決める。`const` は書き込みの可否 |
| 引数を参照にすると、呼び出し側の変更がメンバーに伝わるか | 伝わらない。`_type` は値メンバーなのでコピーされる |
| 引数を `std::string&` にしたら | 一時オブジェクトを束縛できずコンパイルエラー。`const` が必要 |
| `_weapon` を hpp に `= NULL` と書けるか | C++11 なら可。C++98 では初期化リストのみ |
| ポインタの未初期化は生焼けオブジェクトか | その通り。ただし言語は防いでくれないので自分で `NULL` を入れる |
| `NULL` を入れれば設計として健全か | 「対象なし」が正当な状態かどうかが別途問われる |
| 参照が再代入できないのは読み取り専用だからか | 違う。書き込みはできる。できないのは再束縛 |
| 技術的に再束縛が不可能なのか | 不可能ではない。`=` の意味が埋まっていて構文がない |
| ポインタと参照はどちらが先か | ポインタが先。参照は演算子オーバーロードのために後から追加された |
| ex03 の `main` に `{}` だけのブロックがある理由 | 複合文でスコープを分け、2つのテストを独立させ、参照の寿命を安全に保つため |

---

## 16. 参考資料

### 一次資料

| 資料 | 内容 |
|------|------|
| [A History of C++: 1979−1991](https://stroustrup.com/hopl2.pdf) | Stroustrup による設計の経緯。**§3.3.4 References に参照の設計理由と再束縛できない理由**。§3.3.3 は演算子オーバーロードの導入経緯 |
| [The Design and Evolution of C++](https://stroustrup.com/dne.html) | 上記を書籍として拡張したもの |
| [C++ Timeline (dne_notes.pdf)](https://stroustrup.com/dne_notes.pdf) | D&E 付属の年表。月単位の日付 |
| [C++ historical sources archive](https://softwarepreservation.computerhistory.org/c_plus_plus/) | Computer History Museum による原典アーカイブ |
| [Stroustrup: Publications](https://www.stroustrup.com/papers.html) | 論文一覧 |

### 年表（dne_notes.pdf より）

```
1979 May   Work on C with Classes starts
     Oct   1st C with Classes implementation in use
1983 Aug   1st C++ implementation in use
     Dec   C++ named
1984 Jan   1st C++ manual
1985 Feb   1st external C++ release (Release E)
     Oct   Cfront Release 1.0 / The C++ Programming Language
1989 Dec   ANSI X3J16 organizational meeting
1991 Jun   1st ISO WG21 meeting (Lund, Sweden)
```

HOPL-2 §3.3 によれば、C with Classes から C++ を作る際の主要な追加は次の6つ。

> [1] Virtual functions.
> [2] Function name and operator overloading.
> [3] References.
> [4] Constants (const).
> [5] User-controlled free-store memory control.
> [6] Improved type checking

**参照、`const`、`new` / `delete` は同じ時期に同じ方針で追加された機能群**。Module 01 で扱った要素がほぼそのまま並んでいる。

### 各演習の詳細

| ファイル | 内容 |
|----------|------|
| `ex00/README.md` | スタックとヒープ、工場関数、宣言の置き場所 |
| `ex01/README.md` | `new[]` の内部動作、Placement new との比較 |
| `ex02/README.md` | ポインタと参照のアドレス・値、メモリレイアウト |

ex03（参照メンバーとポインタメンバーの設計判断）は本ファイルの §7 と §8 にまとめてある。
