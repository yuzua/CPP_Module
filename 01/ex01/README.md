# CPP Module 01 — ex01: Moar brainz!

後日この課題を見返したときに、文法の要点と当時考えたことを思い出すためのメモ。
課題の手順書ではない。

---

## 目次

1. [課題の目的](#1-課題の目的)
2. [ex00 との違い](#2-ex00-との違い)
3. [ディレクトリ構成](#3-ディレクトリ構成)
4. [実装の要点（方法A）](#4-実装の要点方法a)
5. [new / delete と new[] / delete[]](#5-new--delete-と-new--delete)
6. [なぜデフォルトコンストラクタが必要か](#6-なぜデフォルトコンストラクタが必要か)
7. [低レイヤー：new[] の内部で起きていること](#7-低レイヤーnew-の内部で起きていること)
8. [Placement new との比較](#8-placement-new-との比較)
9. [N 体を作る方法の整理](#9-n-体を作る方法の整理)
10. [Entity としての Zombie と setter](#10-entity-としての-zombie-と-setter)
11. [宣言を main.cpp に置く理由（ex00 からの継続）](#11-宣言を-maincpp-に置く理由ex00-からの継続)
12. [期待される出力](#12-期待される出力)
13. [追加で理解しておくこと](#13-追加で理解しておくこと)
14. [よくある間違い](#14-よくある間違い)
15. [評価で聞かれやすいこと](#15-評価で聞かれやすいこと)
16. [ex02 に向けて](#16-ex02-に向けて)

---

## 1. 課題の目的

ex01 の核心は **「1回の割り当てで N 体のオブジェクト配列を作る」** こと。

| 関数 | 役割 |
|------|------|
| `zombieHorde(int N, std::string name)` | `new[]` で N 体確保し、全員に同じ名前を付けて、先頭へのポインタを返す |

> ex00 が「1体をどこに作るか（スタック vs ヒープ）」なら、ex01 は「N 体をまとめてヒープに作る」。

---

## 2. ex00 との違い

| | ex00 | ex01 |
|---|------|------|
| 確保する数 | 1体 | N 体 |
| キーワード | `new` / `delete` | `new[]` / `delete[]` |
| 工場関数 | `newZombie`, `randomChump` | `zombieHorde` |
| 返り値 | そのゾンビへのポインタ | **先頭（1体目）** へのポインタ |
| アクセス | `zombie->announce()` | `horde[i].announce()` |
| Zombie クラス | 名前付き ctor のみ | **デフォルト ctor + setName 追加** |

---

## 3. ディレクトリ構成

```
ex01/
├── Zombie.hpp        … Zombie クラス（ex00 + デフォルト ctor / setName）
├── Zombie.cpp
├── zombieHorde.cpp   … N 体を new[] で確保する工場関数
├── main.cpp          … テスト + zombieHorde の宣言
├── Makefile
└── README.md
```

ex00 にあった `newZombie.cpp` / `randomChump.cpp` は **不要**。

---

## 4. 実装の要点（方法A）

採用した方針：**`new[]` + デフォルトコンストラクタ + `setName`**

### Zombie クラス（ex00 からの追加点）

```cpp
class Zombie {
private:
    std::string _name;
public:
    Zombie();                          // 追加：new[] 用
    Zombie(const std::string& name);   // ex00 から継続
    ~Zombie();
    void setName(const std::string& name);  // 追加
    void announce(void) const;
};
```

### zombieHorde.cpp

```cpp
Zombie* zombieHorde(int N, std::string name) {
    Zombie* horde = new Zombie[N];
    for (int i = 0; i < N; i++)
        horde[i].setName(name);
    return horde;
}
```

### main.cpp

```cpp
#include "Zombie.hpp"

Zombie* zombieHorde(int N, std::string name);

int main(void) {
    const int N = 5;

    Zombie* horde = zombieHorde(N, "Bob");

    for (int i = 0; i < N; i++)
        horde[i].announce();

    delete[] horde;

    return 0;
}
```

### なぜ方法Aか

| 方法 | 内容 | ex01 で採用 |
|------|------|------------|
| **A** | `new[]` + デフォルト ctor + `setName` | **◎ 採用** |
| B | `new[]` + デフォルト ctor + 代入 `horde[i] = Zombie(name)` | ○ 可能 |
| C | Placement new + 名前付き ctor のみ | △ 理解用。ex01 要件を満たすが過剰 |
| D | `std::vector` | × Module 01 禁止 |

---

## 5. new / delete と new[] / delete[]

### 対応関係

```
new     ↔  delete      … 1体
new[]   ↔  delete[]    … 配列（N 体）
```

**混ぜると未定義動作（クラッシュ・リークの原因）。**

```cpp
Zombie* horde = new Zombie[N];
delete horde;    // NG
delete[] horde;  // OK
```

### なぜペアが必要か（低レイヤー）

`new[]` は通常、確保したメモリの **直前（または内部）** に「要素数」などの情報を記録する。  
`delete[]` はその情報を読んで **N 回デストラクタ + 1 回メモリ解放** する。

`delete`（単体版）を配列に使うと、**1 体分しか破棄されない** → リークや二重解放。

```
new[] のイメージ
┌──────────────────────────────────────┐
│ [ メタ情報 ] │ Zombie₀ │ Zombie₁ │ ... │
└──────────────────────────────────────┘
                ↑
              horde（先頭要素のアドレス）

delete[] horde
  → Zombie₀ の dtor
  → Zombie₁ の dtor
  → ...
  → ブロック全体を解放
```

---

## 6. なぜデフォルトコンストラクタが必要か

```cpp
Zombie* horde = new Zombie[N];
```

これは内部で **引数なし** でコンストラクタを N 回呼ぶ。

```
horde[0] の ctor（引数なし）
horde[1] の ctor（引数なし）
...
horde[N-1] の ctor（引数なし）
```

C++98 では `new Zombie[N](name)` のように配列全体に引数を渡せない。  
だから **デフォルト ctor で空の箱を N 個作り、あとから `setName` で名前を入れる**。

### 代入で上書きする場合も同じ

```cpp
Zombie* horde = new Zombie[N];       // ① デフォルト ctor が N 回（必須）
for (int i = 0; i < N; i++)
    horde[i] = Zombie(name);        // ② 代入で上書き
```

① の時点でデフォルト ctor が必要。setter も代入も **根本は同じ**。

---

## 7. 低レイヤー：new[] の内部で起きていること

通常の `new` と同様、2 段階に分けて考えられる。

```
① operator new[]  … N * sizeof(Zombie) バイトを1ブロック確保
② 各スロットでコンストラクタ … horde[0] 〜 horde[N-1]
```

### メモリ上のイメージ

```
ヒープ（1ブロック）
┌──────────┬──────────┬──────────┬─────┐
│ Zombie₀  │ Zombie₁  │ Zombie₂  │ ... │
│ _name="" │ _name="" │ _name="" │     │  ← デフォルト ctor 直後
└──────────┴──────────┴──────────┴─────┘
↑
horde

setName("Bob") 後
┌──────────┬──────────┬──────────┬─────┐
│ _name=   │ _name=   │ _name=   │     │
│ "Bob"    │ "Bob"    │ "Bob"    │     │
└──────────┴──────────┴──────────┴─────┘
```

### storage と object

| 段階 | 状態 |
|------|------|
| `operator new[]` 直後 | バイト列のみ（未構築） |
| デフォルト ctor 後 | 空の `_name` を持つ Zombie オブジェクト |
| `setName` 後 | `_name = "Bob"` の Zombie オブジェクト |
| `delete[]` 後 | すべて破棄・メモリ返却 |

---

## 8. Placement new との比較

### 通常の new[]（ex01 で採用）

```cpp
Zombie* horde = new Zombie[N];
for (int i = 0; i < N; i++)
    horde[i].setName(name);
```

```
① 確保 + ② デフォルト ctor × N  →  ③ setName
```

### Placement new（理解用・ex01 では未採用）

```cpp
void* raw = ::operator new(N * sizeof(Zombie));
Zombie* horde = static_cast<Zombie*>(raw);
for (int i = 0; i < N; i++)
    new (horde + i) Zombie(name);   // 名前付き ctor を直接呼べる
```

```
① 確保のみ  →  ② 名前付き ctor × N（デフォルト ctor 不要）
```

| | new[] | Placement new |
|---|-------|---------------|
| デフォルト ctor | **必要** | **不要** |
| 名前付き ctor を直接使える | ×（C++98） | ○ |
| コードの複雑さ | 低 | 高 |
| ex01 | **採用** | 理解用 |

> **Placement new = 土地（メモリ）と家（コンストラクタ）を分けて制御できる。**  
> **new[] = それを言語がまとめてやってくれる糖衣包装。**

---

## 9. N 体を作る方法の整理

| 方法 | 1回割り当て | ex01 要件 | Module 01 |
|------|------------|-----------|-----------|
| `new[]` / `delete[]` | ○ | **◎** | ○ |
| `new` × N 回 | × | × | ○ |
| `std::vector` | △ | × | **×** |
| スタック配列 + 返却 | ○ | ×（返せない） | ○ |
| Placement new | ○ | △ | ○ |

### なぜ課題は new[] を選ぶか

1. ex00 の `new` / `delete` からの自然な拡張
2. 生ポインタと手動メモリ管理の体験
3. 「1回の割り当て」の理解（断片化 vs 連続ブロック）
4. STL 禁止の段階で配列の基礎を固める

現場では `std::vector` が一般的だが、Module 01 では **`new[]` で下の仕組みを理解する** 段階。

---

## 10. Entity としての Zombie と setter

### Entity / Value Object の整理

| 対象 | 分類 |
|------|------|
| `_name`（名前） | 値オブジェクト寄り |
| `Zombie` インスタンス | エンティティ寄り |

### setter を使う理由

**Entity だから setter** ではなく、**`new[]` の制約** から setter（または代入）を選んでいる。

```
Entity だから setter     →  ×（ex00 では ctor で名前を渡していた）
new[] だから setter/代入 →  ○（正確な理由）
```

ex00 の `newZombie("Foo")` も Entity 的な使い方で、コンストラクタで名前を渡していた。  
ex01 では **`new[]` がデフォルト ctor を要求する** ため、方法A（setter）を採用。

---

## 11. 宣言を main.cpp に置く理由（ex00 からの継続）

ex00 と同様、`zombieHorde` は **フリー関数** として `zombieHorde.cpp` に実装。  
宣言は **追加 .hpp を作らず `main.cpp` に記述**。

```
Zombie.hpp / Zombie.cpp  … クラス専用
zombieHorde.cpp          … 工場関数の実装
main.cpp                 … テスト + zombieHorde の宣言
```

---

## 12. 期待される出力

```
Bob: BraiiiiiiinnnzzzZ...
Bob: BraiiiiiiinnnzzzZ...
Bob: BraiiiiiiinnnzzzZ...
Bob: BraiiiiiiinnnzzzZ...
Bob: BraiiiiiiinnnzzzZ...
Bob is destroyed
Bob is destroyed
Bob is destroyed
Bob is destroyed
Bob is destroyed
```

- `announce()` が N 回
- デストラクタが N 回（`delete[]` 時）

### 42 環境での確認

```bash
make
./MoarBrainz
valgrind --leak-check=full ./MoarBrainz
```

---

## 13. 追加で理解しておくこと

### 配列ポインタと添字アクセス

```cpp
Zombie* horde = zombieHorde(N, "Bob");
horde[2].announce();   // 3体目
*(horde + 2)           // horde[2] と同じ
```

返されたポインタは **先頭要素のアドレス**。C の配列と同じ感覚。

### デフォルトコンストラクタと空の string

```cpp
Zombie::Zombie() {}  // _name は空文字列 "" で初期化される
```

`std::string _name` はデフォルト ctor で空になる。  
`setName` 前の `announce()` を呼ぶと `: BraiiiiiiinnnzzzZ...` とだけ出る（テスト時の注意）。

### コピー代入（方法B）

```cpp
horde[i] = Zombie(name);
```

暗黙のコピー代入演算子が使われる。ex01 では方法A（setter）を採用したが、方法B も要件上は問題ない。

### cout の `+` と `<<`

```cpp
std::cout << this->_name + ": BraiiiiiiinnnzzzZ..." << std::endl;  // 一時 string を作る
std::cout << this->_name << ": BraiiiiiiinnnzzzZ..." << std::endl; // そのまま流す
```

どちらも正しい。`<<` 連鎖の方が iostream では一般的。

### RAII との関係

`new[]` / `delete[]` は RAII が効かない。  
Module 04 以降でスマートポインタやコンテナが登場し、自動解放の世界へ進む。

---

## 14. よくある間違い

| 問題 | 原因 |
|------|------|
| `delete` で配列を解放 | `delete[]` を使う |
| `new` を N 回 | 1回の割り当て要件に違反 |
| デフォルト ctor なしで `new Zombie[N]` | コンパイルエラー |
| `setName` 未実装 | ヘッダ/実装の不一致 |
| `main` で `N` 未定義 | `const int N = 5;` 等を定義する |
| `std::vector` 使用 | Module 01 禁止 |
| メモリリーク | `delete[]` 忘れ |
| 存在しないヘッダ include | ex00 と同様の落とし穴 |

---

## 15. 評価で聞かれやすいこと

- **`new` と `new[]` の違いは？**
- **`delete` と `delete[]` の違いは？混ぜると？**
- **なぜデフォルトコンストラクタが必要？**
- **Placement new ならデフォルト ctor 不要な理由は？**
- **返り値が「先頭へのポインタ」である意味は？**
- **`horde[i]` と `*(horde + i)` の関係は？**
- **なぜ N 回 `new` ではダメ？**
- **Entity なのに setter を使う理由は？**（→ `new[]` の制約）

---

## 16. ex02 に向けて

ex02「HI THIS IS BRAIN」では **クラス不要**。  
`main.cpp` だけで **ポインタと参照** のアドレス・値を出力する。

ex01 で触れた内容との接続:

| ex01 | ex02 |
|------|------|
| `Zombie*` ポインタ | `std::string*` ポインタ |
| `horde[i]` アクセス | 参照 `stringREF` |
| ヒープ上のアドレス | スタック上の `str` のアドレス |

> 参照は「別物」ではなく、**同じメモリへの別名（エイリアス）**。
