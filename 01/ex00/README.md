# CPP Module 01 — ex00: BraiiiiiiinnnzzzZ

後日この課題を見返したときに、文法の要点と当時考えたことを思い出すためのメモ。
課題の手順書ではない。

---

## 目次

1. [課題の目的](#1-課題の目的)
2. [ディレクトリ構成と役割分担](#2-ディレクトリ構成と役割分担)
3. [スタックとヒープ](#3-スタックとヒープ)
4. [実装の要点](#4-実装の要点)
5. [宣言と定義 — なぜ main.cpp に書くのか](#5-宣言と定義--なぜ-maincpp-に書くのか)
6. [Zombie クラスの設計判断](#6-zombie-クラスの設計判断)
7. [値オブジェクト vs エンティティ](#7-値オブジェクト-vs-エンティティ)
8. [アドレス出力による確認（学習用）](#8-アドレス出力による確認学習用)
9. [期待される出力](#9-期待される出力)
10. [追加で理解しておくこと](#10-追加で理解しておくこと)
11. [よくある間違い](#11-よくある間違い)
12. [評価で聞かれやすいこと](#12-評価で聞かれやすいこと)
13. [ex01 に向けて](#13-ex01-に向けて)

---

## 1. 課題の目的

ex00 の本当の目的は「ゾンビを作る」ことではなく、**オブジェクトをどこに作るか（スタック vs ヒープ）** を理解すること。

| 関数 | 割り当て先 | 寿命 | 破棄 |
|------|-----------|------|------|
| `newZombie()` | ヒープ（`new`） | 関数を抜けても残る | 呼び出し側が `delete` |
| `randomChump()` | スタック（ローカル変数） | 関数内だけ | 関数終了時に自動 |

> 長く使うゾンビは `new` で作って返す。一時的なゾンビは関数内のローカル変数にする。不要になったら必ず破棄する。

---

## 2. ディレクトリ構成と役割分担

```
ex00/
├── Zombie.hpp        … Zombie クラスの宣言（クラス専用）
├── Zombie.cpp        … コンストラクタ、デストラクタ、announce()
├── newZombie.cpp     … ヒープにゾンビを作る工場関数
├── randomChump.cpp   … スタックにゾンビを作って叫ばせる工場関数
├── main.cpp          … テスト + 工場関数の宣言
├── Makefile
└── README.md
```

### 関心の分離

| 対象 | 関心事 |
|------|--------|
| `Zombie` クラス | ゾンビ1体の振る舞い（名前、叫ぶ、生/death） |
| `newZombie()` | ヒープ上にゾンビを作る（クラスの外） |
| `randomChump()` | スタック上に一時的なゾンビを作る（クラスの外） |

`newZombie` / `randomChump` は **メンバ関数ではない**。ゾンビを「作る」工場として、別 `.cpp` に切り出している。

---

## 3. スタックとヒープ

### スタック（Stack）

```cpp
void randomChump(std::string name) {
    Zombie zombie(name);  // スタック上に作られる
    zombie.announce();
}   // ← ここで zombie は自動破棄（デストラクタが呼ばれる）
```

- 関数を抜けると**自動で消える**
- 関数の外には持ち出せない
- `delete` は不要

### ヒープ（Heap）

```cpp
Zombie* newZombie(std::string name) {
    return new Zombie(name);  // ヒープ上に作られる
}
// 関数を抜けてもオブジェクトは残る
// 使い終わったら delete が必要
```

- 関数を抜けても**オブジェクトは残る**
- ポインタで外に渡せる
- 使い終わったら **`delete` しないとメモリリーク**

### メモリ配置のイメージ

```
高アドレス
┌─────────────────────────┐
│  スタック (Stack)        │
│  main のローカル変数      │
│  randomChump の zombie   │  ← 0x7ffe... 付近
├─────────────────────────┤
│         ...             │
├─────────────────────────┤
│  ヒープ (Heap)           │
│  new で確保した zombie   │  ← 0x55... 付近
└─────────────────────────┘
低アドレス
```

---

## 4. 実装の要点

### Zombie クラス

```cpp
// Zombie.hpp
class Zombie {
private:
    std::string _name;
public:
    Zombie(const std::string& name);
    ~Zombie();
    void announce(void) const;
};
```

```cpp
// Zombie.cpp
Zombie::Zombie(const std::string& name) : _name(name) {}

Zombie::~Zombie() {
    std::cout << this->_name + " is destroyed" << std::endl;
}

void Zombie::announce(void) const {
    std::cout << this->_name + ": BraiiiiiiinnnzzzZ..." << std::endl;
}
```

### newZombie（ヒープ）

```cpp
Zombie* newZombie(std::string name) {
    return new Zombie(name);
}
```

### randomChump（スタック）

```cpp
void randomChump(std::string name) {
    Zombie zombie(name);
    zombie.announce();
}
```

### main（テスト）

```cpp
#include "Zombie.hpp"

Zombie* newZombie(std::string name);
void randomChump(std::string name);

int main(void) {
    Zombie* zombie = newZombie("HeapZombie");
    zombie->announce();
    delete zombie;

    randomChump("StackZombie");

    return 0;
}
```

---

## 5. 宣言と定義 — なぜ main.cpp に書くのか

### コンパイルの2段階

```
1. コンパイル（ファイルごと）
   main.cpp      → main.o
   newZombie.cpp → newZombie.o

2. リンク（.o をつなげる）
   main.o + newZombie.o + ... → 実行ファイル
```

`main.cpp` をコンパイルするとき、コンパイラは `newZombie.cpp` を**読まない**。
だから `main.cpp` には「`newZombie` という関数がある」という**宣言**が必要。

### 実装の分離 vs 宣言の置き場所

| 概念 | 説明 |
|------|------|
| `.cpp` に実装を分ける | 関心の分離（設計） |
| どこかに宣言を書く | コンパイラへの通知（C++ のルール） |

**別 `.cpp` に実装を分けても、宣言は自動的には見えない。**

### 宣言を置く選択肢（追加 .hpp なし）

| 方法 | 内容 | 採用 |
|------|------|------|
| `main.cpp` に宣言 | 呼び出し元だけが知ればよい | **今回採用** |
| `Zombie.hpp` に宣言 | 1ヘッダですべて公開 | 42 ではよく見る |
| 別 `.hpp` を作る | 設計上きれい | 提出ファイルにないので未採用 |

今回は **関心の分離** を優先し、`Zombie.hpp` はクラス専用、`main.cpp` に工場関数の宣言を置いた。

### 依存関係

```
main.cpp ──────────→ Zombie.hpp
                 └── （工場関数の宣言は自分が持つ）

newZombie.cpp ────→ Zombie.hpp
randomChump.cpp ──→ Zombie.hpp
Zombie.cpp ───────→ Zombie.hpp
```

`Zombie.cpp` は工場関数を知る必要がない。

---

## 6. Zombie クラスの設計判断

### コンストラクタ: `const std::string& name`

```cpp
Zombie(const std::string& name);  // 参照渡し（ポインタではない）
```

| 書き方 | コピー回数 |
|--------|-----------|
| `std::string name`（値渡し） | 引数 + `_name` で 2回 |
| `const std::string& name`（参照渡し） | `_name` への代入で 1回 |

`const` は「参照先を変更しない」という約束。ex00 規模では値渡しでも動くが、**大きなオブジェクトは `const T&` で受け取る**のが C++ の慣習。

### `_name` は const にしない

- コンストラクタの初期化リスト `: _name(name)` で設定する
- `const` にしても初期化リストなら動くが、ex00 では名前変更は不要かつ将来の `setName()` も想定しない
- 課題も `const` を要求していない → **`std::string _name` で十分**

### `announce() const`

```cpp
void announce(void) const;
```

| | const なし | const あり |
|---|-----------|-----------|
| 実行結果 | 同じ | 同じ |
| メンバ変数の変更 | 可能（書けば） | **コンパイルエラー** |
| const オブジェクトから呼べる | 不可 | 可能 |

`announce()` は `_name` を読むだけなので `const` を付けるのが自然。実行時の挙動は変わらないが、**意図の表明**と**間違い防止**になる。

---

## 7. 値オブジェクト vs エンティティ

| 種類 | 同一性 | 変更 |
|------|--------|------|
| **値オブジェクト** | 属性で同じなら同じ | 基本は不変。変えるなら新インスタンス |
| **エンティティ** | ID 等で区別 | 属性を更新しても同じ個体 |

### ex00 に当てはめると

| 対象 | 分類 | 理由 |
|------|------|------|
| `_name`（名前） | 値オブジェクト寄り | 属性だけで意味が決まる |
| `Zombie` インスタンス | エンティティ寄り | 個体・寿命・メモリ上の同一性がある |

同じ `"Foo"` でも `newZombie("Foo")` を2回呼べば **2体の別ゾンビ** になる。デストラクタも「この個体が死んだ」と表現する。

> **「ゾンビの名前」は値オブジェクト的。「ゾンビ1体」はエンティティ的。**

ex00 は DDD の課題ではなく、**メモリ上の1体として生まれて死ぬオブジェクト**として理解するのが課題の意図に近い。

---

## 8. アドレス出力による確認（学習用）

**提出コードには含めない。** 42 環境で一時的に試す用。

### main.cpp（デバッグ版）

```cpp
#include "Zombie.hpp"
#include <iostream>

Zombie* newZombie(std::string name);
void randomChump(std::string name);

int main(void) {
    int stackMarker = 0;
    std::cout << "[main]   stack marker:  " << &stackMarker << std::endl;

    Zombie* zombie = newZombie("HeapZombie");
    std::cout << "[main]   pointer var:   " << &zombie << " (stack)" << std::endl;
    std::cout << "[main]   heap object:  " << static_cast<void*>(zombie) << " (heap)" << std::endl;

    zombie->announce();
    delete zombie;

    randomChump("StackZombie");

    return 0;
}
```

### randomChump.cpp（デバッグ版）

```cpp
#include "Zombie.hpp"
#include <iostream>

void randomChump(std::string name) {
    int stackMarker = 0;
    Zombie zombie(name);

    std::cout << "[chump]  stack marker:  " << &stackMarker << " (stack)" << std::endl;
    std::cout << "[chump]  zombie object: " << &zombie << " (stack)" << std::endl;

    zombie.announce();
}
```

### 見るポイント

| 書き方 | 意味 |
|--------|------|
| `zombie`（ポインタの値） | ヒープ上の Zombie 本体のアドレス |
| `&zombie`（main のポインタ変数） | スタック上 |
| `&zombie`（randomChump のローカル） | スタック上 |

スタックは `0x7ffe...` 付近、ヒープは `0x55...` 付近など**別のアドレス帯**になる（環境依存）。

### valgrind

```bash
make
./BraiiiiiiinnnzzzZ
valgrind --leak-check=full ./BraiiiiiiinnnzzzZ
```

`All heap blocks were freed -- no leaks are possible` が出れば、`delete` 漏れなし。

---

## 9. 期待される出力

```
HeapZombie: BraiiiiiiinnnzzzZ...
HeapZombie is destroyed
StackZombie: BraiiiiiiinnnzzzZ...
StackZombie is destroyed
```

| メッセージ | タイミング |
|-----------|-----------|
| `HeapZombie is destroyed` | `delete` 時 |
| `StackZombie is destroyed` | `randomChump` 終了時（自動） |

---

## 10. 追加で理解しておくこと

### new / delete（C++ の動的メモリ）

- C の `malloc` / `free` 相当だが、**コンストラクタ / デストラクタが自動で呼ばれる**
- `new Zombie(name)` → ヒープ確保 + コンストラクタ
- `delete zombie` → デストラクタ + メモリ解放
- **`malloc` / `free` / `*printf` は課題で禁止**

### ポインタと参照

| | ポインタ `T*` | 参照 `T&` |
|---|--------------|-----------|
| 再代入 | 可能 | 不可（常に同じ対象） |
| null | あり得る | なし |
| 構文 | `->` / `*` | `.` でそのまま |

```cpp
zombie->announce();  // ポインタ経由
zombie.announce();   // オブジェクト直接
```

### 初期化リスト

```cpp
Zombie::Zombie(const std::string& name) : _name(name) {}
//                                      ^^^^^^^^^^^^^
//                                      メンバを初期化（代入より推奨）
```

C++ ではメンバは初期化リストで初期化するのが基本。本体 `{ _name = name; }` より効率がよい。

### RAII（Resource Acquisition Is Initialization）

- オブジェクトの寿命 = リソースの寿命
- スタックのローカル変数はスコープを抜けると自動破棄 → **RAII の基本**
- ヒープは自分で `delete` する責任がある → ex01 以降で `delete[]` も登場

### コンパイルモデル（再確認）

```
宣言（.hpp や main.cpp）  → コンパイラに「こういう関数がある」と知らせる
定義（.cpp）              → 実際の処理。リンク時に呼び出しと接続
```

`.cpp` を `#include` するのは非推奨（二重定義でリンクエラーになりやすい）。

### C++98 の制約

- `-std=c++98` でコンパイル必須
- C++11 以降の機能（`auto`、範囲 for、nullptr 等）は使えない
- STL コンテナ（vector 等）は Module 08 まで禁止

---

## 11. よくある間違い

| 問題 | 原因 |
|------|------|
| `newZombie` が未定義 | `main.cpp` に宣言がない |
| 存在しない `.hpp` を include | 別ヘッダを作らなかったのに include だけした |
| `#include "Zombie.cpp"` | `.cpp` は include しない |
| メモリリーク | `new` したのに `delete` していない |
| Makefile が動かない | `OBJS` の typo（`OJS` 等） |
| インクルードガードなし | 二重 include でエラー |
| `using namespace std` | 禁止（-42 点） |

---

## 12. 評価で聞かれやすいこと

- **なぜ `newZombie` はヒープ、`randomChump` はスタック？**
  - 寿命が関数スコープを超えるかどうか
- **`delete` を忘れると？** → メモリリーク
- **デストラクタはいつ呼ばれる？**
  - スタック: スコープ終了時
  - ヒープ: `delete` 時
- **`.hpp` と `.cpp` の役割の違いは？**
- **なぜ `newZombie` はクラスのメンバ関数ではない？**
  - まだ存在しないオブジェクトを作るから
- **`const std::string&` と `std::string` の違いは？**
- **`announce() const` の意味は？**

---

## 13. ex01 に向けて

ex01 では `zombieHorde(int N, std::string name)` を実装する。

- **一度の割り当てで N 個** → `new Zombie[N]`（配列）
- 各ゾンビに同じ名前を付けて初期化
- 最初のゾンビへのポインタを返す
- **`delete[]` で解放**（`delete` ではない）

ex00 で学んだ `new` / `delete` が、配列版に拡張される。
