# ex05 Architecture — Harl 2.0

CPP Module 01 ex05 の設計メモ。課題を「何を作るか」「なぜ `Harl` クラスとメンバ関数ポインタか」から整理する。

---

## このプログラムは何をするか

**Harl** は、レベル名（文字列）を渡すと、決まった「文句」を標準出力に出すキャラクター。

| レベル | 意味（subject の説明） |
|--------|------------------------|
| `DEBUG` | 問題の診断用。状況の詳細 |
| `INFO` | 本番でも役立つ、実行のトレース用 |
| `WARNING` | 潜在的な問題。無視もできる |
| `ERROR` | 回復不能なエラー。人手が必要 |

呼び出しのイメージ:

```cpp
Harl harl;
harl.complain("DEBUG");   // → DEBUG 用の決まったセリフ
harl.complain("ERROR");   // → ERROR 用の決まったセリフ
```

課題は **4 種類の private メンバ関数** にセリフを書き、**public の `complain(level)` だけ** から呼び分けること。  
**メンバ関数ポインタ** を使い、**`if / else if / else` の森** でレベルを分岐しない。

---

## 課題のルール（守ること）

| 項目 | 内容 |
|------|------|
| 提出 | `Makefile`, `main.cpp`, `Harl.{h,hpp}`, `Harl.cpp` |
| **private** | `void debug(void);`, `info`, `warning`, `error` |
| **public** | `void complain(std::string level);`（実装では `const &` でも可） |
| **必須** | **ポインタ to メンバ関数** で dispatch |
| **必須** | `if/else if/else` の森でレベル分岐 **しない** |
| **必須** | 自作テストで「Harl がよく文句を言う」ことを示す |
| 禁止 | （subject 上）特になし |

セリフは subject の例文でも、自分の文でもよい。

---

## 設計で決めた細かいルール（subject 未指定分）

| ルール | 内容 |
|--------|------|
| **未知の level** | 例: `"NOPE"`, `""`, `"debug"`（小文字）→ **何も出力せず、正常終了** |
| **大文字小文字** | **完全一致**（`"DEBUG"` のみ有効） |
| **出力形式** | 各 handler は `[ DEBUG ]` のような見出し行 + 本文（テストしやすく、ex06 と揃えやすい） |
| **コンストラクタ** | **何も出力しない**（テストの oracle を汚さない） |

校内で別ルールが指定されたら、ここだけ更新する。

---

## 全体の流れ

```text
  main.cpp
     │  Harl を1つ作る
     │  complain("DEBUG") などを呼ぶ（テスト用）
     ▼
  Harl::complain(level)
     │  表を1行ずつ見て level と一致する行を探す
     │  見つかった → その行の「メンバ関数ポインタ」を (this->*...)() で実行
     │  見つからない → 何もしない
     ▼
  Harl::debug / info / warning / error（private）
     │  決まったセリフを std::cout へ
     ▼
  標準出力
```

**ポイント**: `main` は **レベル名だけ** 知ればよい。どの関数が DEBUG 用か、中身の英文は **`Harl.cpp` の中だけ**。

---

## ファイル構成

```text
01/ex05/
├── architecture.md   ← この文書
├── Makefile
├── main.cpp          # 各レベル・未知 level の呼び出し（テストドライバ）
├── Harl.hpp          # クラス宣言（公開面）
├── Harl.cpp          # セリフ + dispatch 表
└── tests/
    ├── run_tests.sh
    └── expected_output.txt   # 推奨: 全文 diff 用
```

---

## 各ファイルの役割

### `Harl.hpp` — 外から見える約束

- **public**: `complain` だけ（＋ コンストラクタ / デストラクタ）
- **private**: `debug`, `info`, `warning`, `error` の **宣言のみ**
- **含めない**: セリフの文字列、dispatch の表、`<iostream>`

```cpp
class Harl {
private:
    void debug(void);
    void info(void);
    void warning(void);
    void error(void);

public:
    Harl(void);
    ~Harl(void);
    void complain(std::string const &level);
};
```

`debug()` を `main` から直接呼ぼうとすると **コンパイルエラー** になる。これが「入口を1つに絞る」設計。

---

### `Harl.cpp` — セリフと dispatch

**1. 各 private メンバ関数**  
subject の例（または自作）を `std::cout` で出力。

**2. `complain` — 表駆動 + メンバ関数ポインタ**

設計上の推奨形（**対応表を1本** にまとめる）:

```cpp
void Harl::complain(std::string const &level) {
    typedef void (Harl::*ComplaintHandler)(void);
    struct Entry {
        const char *level;
        ComplaintHandler handler;
    };

    static const Entry table[] = {
        {"DEBUG",   &Harl::debug},
        {"INFO",    &Harl::info},
        {"WARNING", &Harl::warning},
        {"ERROR",   &Harl::error}
    };
    const std::size_t count = sizeof(table) / sizeof(table[0]);

    for (std::size_t i = 0; i < count; ++i) {
        if (level == table[i].level) {
            (this->*table[i].handler)();
            return;
        }
    }
    // 一致なし → 無出力
}
```

| 記法 | 意味 |
|------|------|
| `void (Harl::*)(void)` | 「引数なしの Harl メンバ関数」を指す型 |
| `&Harl::debug` | メンバ関数のアドレス（**this はまだ含まない**） |
| `(this->*handler)()` | このオブジェクトに対してその関数を呼ぶ |

**避けるパターン（課題の spirit に反する）**:

```cpp
if (level == "DEBUG")
    debug();
else if (level == "INFO")
    info();
// ... 以下 else if だらけ
```

**許容されるが、設計では弱いパターン**:

- `levels[4]` と `funcs[4]` の **2本配列** + `for` で添字を揃える  
  → 動くが、レベル追加時に **2箇所** を揃える必要があり、表1本より壊しやすい。

---

### `main.cpp` — テストドライバ

課題は「Harl がよく文句を言う」テストの提出を求める。

**最低限呼ぶとよい level**:

1. `"DEBUG"`, `"INFO"`, `"WARNING"`, `"ERROR"`（各1回以上）
2. 未知: `"NOPE"` など
3. 空文字 `""`、小文字 `"debug"`（設計方針の確認用）

`main` に **英文を書かない**（文言はすべて `Harl.cpp`）。

---

### `Makefile`

- `-Wall -Wextra -Werror -std=c++98`
- `NAME = harl`（慣習）
- `SRCS = main.cpp Harl.cpp`
- 任意: `test` ターゲットで `tests/run_tests.sh`

---

## なぜ ex04 ではクラスにしなかったのに、ここでは `Harl` クラスか

| 観点 | ex04 置換 | ex05 Harl |
|------|-----------|-----------|
| subject | クラス指定なし | **クラス必須** |
| インスタンス変数 | なし | なし（状態は持たない） |
| 守りたいもの | なし（純粋関数で足りる） | **4つの文言関数を private に閉じる**（情報隠蔽） |
| 学習テーマ | 文字列・ファイル | **メンバ関数ポインタ** |

状態のためではなく、

1. **入口を `complain` 1本に限定**（`debug()` を外から呼べない）  
2. **メンバ関数のアドレス** を表に載せる（普通の関数ポインタとは別物）

ためにクラスが意味を持つ。詳細は `../ex04/class.md` の「6つの目的」の **目的3（情報隠蔽）** と **subject 指定** の組み合わせ。

---

## テスト（tests/）

### 契約の oracle

| ID | 内容 |
|----|------|
| T-01〜04 | 各 level で対応する見出し・本文が出る |
| T-05 | 1 level だけ呼んだとき、**他 level の文言が混ざらない** |
| T-06〜08 | 未知 / 空 / 小文字 → **出力なし** |
| T-09 | 4 level を順に複数回 → 順序・回数が期待どおり |

### 推奨: 出力全文の golden file

`./harl > actual.txt` と `tests/expected_output.txt` を **`diff`** する。

- 「文字列が含まれる」だけのテストより、**排他性（T-05）** も検証しやすい  
- 未知 level で **余計な行が増えていない** ことも分かる  

---

## ex06 へのつながり（Harlem filter）

ex06 では「指定 level **以上** の文句を、下の level から順に全部出す」。

今回の **表の並び順** を重大度順（DEBUG → INFO → WARNING → ERROR）にしておくと、

- `complain` で「一致した行の index 以降」をループする  
- または switch で index から分岐  

へ拡張しやすい。**表1本 + 順序** は ex06 の設計資産になる。

---

## 設計の根拠（skill との対応）

| 観点 | ex05 での置き方 |
|------|-----------------|
| Problem framing | レベル文字列 → 固定文句。分岐の森を避ける |
| Domain model | 閉じた4 level、DispatchTable、Handler 型 |
| Design by contract | `complain` の Pre/Post、未知 level は無出力 |
| Interface / implementation | `.hpp` = 入口のみ、表と文言は `.cpp` |
| Architecture | 単一クラス CLI — 大規模構成は不要 |

---

## Defense / 提出前チェックリスト

- [ ] `complain` が **メンバ関数ポインタ** で dispatch している  
- [ ] レベル分岐が **`if / else if / else` の長い鎖** になっていない  
- [ ] `debug` 等が **private** で、`main` から直接呼べない  
- [ ] 4 level のセリフが出る（テスト or 実行で確認）  
- [ ] 未知 level の挙動を説明できる（本設計: 無出力）  
- [ ] メンバ関数ポインタの呼び方 `(this->*p)()` を説明できる  
- [ ] `make` が `-Werror` で通る  

---

## 関連ファイル

| ファイル | 用途 |
|----------|------|
| `architecture.md` | 本設計（初学者向け） |
| `../ex04/class.md` | クラスを使う／使わない判断 |
| `../ex04/architecture.md` | ex04 設計（対比用） |
