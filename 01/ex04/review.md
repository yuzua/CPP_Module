# ex04 Design Review

`_` なし版（提出名: `main.cpp`, `replace.hpp`, `replace.cpp`, `Makefile`, `tests/`）を、skill ベースの設計（`architecture.md`）と照らしてレビューした記録。

---

## 1. レビュー対象

| 対象 | パス |
|------|------|
| 提出名実装 | `main.cpp`, `replace.hpp`, `replace.cpp`, `Makefile`, `tests/` |
| 参照実装 | `main_.cpp`, `replace_.hpp`, `replace_.cpp`, `Makefile_`, `tests_/` |
| 設計正本 | `architecture.md`, `class.md` |

---

## 2. `_` あり / `_` なしの違い

### 2.1 結論（設計レベル）

**設計・ロジックは同一**である。skill の境界（CLI / 純粋置換 / I/O 分離）も同じ。

| 観点 | `_` なし | `_` あり |
|------|----------|----------|
| ファイル名 | subject の提出要件に一致 | 提出には使えない（参照用） |
| 変数名 | `file_name`, `file_content`, `replaced_content`, `out_path` | `filename`, `content`, `replaced`, `outPath` |
| `main.cpp` 体裁 | 行間に空行が多い（可読性・norm 上の改善余地） | 通常の密度 |
| 置換アルゴリズム | `find` + `append`、非重複左から | 同一 |
| テスト | `tests/run_tests.sh` | `tests_/run_tests_.sh`（ケース同等） |

### 2.2 skill に基づく「どちらが良いか」

- **設計の適合度**: 引き分け（同一）。
- **提出・評価**: **`_` なし** のみが要件を満たす。
- **コードスタイル**: Module 00/01 の慣習（camelCase 系）との一貫性では、`_` あり側のローカル変数名に寄せた方がリポジトリ内で揃いやすい（設計判断ではなくスタイル）。
- **ドキュメント**: `architecture.md` は `_` 付き参照実装を明示している。提出前に `_` 付き一式を削除または別置きする必要あり（余剰 `main_.cpp` 等は提出物に含めない）。

---

## 3. Skill 観点の総合判定

| Skill / 観点 | 判定 | コメント |
|--------------|------|----------|
| Interface / Implementation Separation | **separated** | `replace.hpp` が意味契約、`.cpp` がアルゴリズム、`main` が I/O |
| Design by Contract | **conditional** | 主要 UC は満たすが、読取失敗検知（F1）とテスト網（F3）に穴 |
| Domain Model | **sufficient** | 状態なし純粋置換；クラス不要の判断は `class.md` と一致 |
| Problem Framing | **ready** | subject の CLI・禁止 API・出力名に沿っている |

---

## 4. 妥当な点（変更不要）

### 4.1 境界と責務

- `replaceAll` はファイル I/O を知らない **純粋関数**。
- 禁止: `std::string::replace`（メンバ）、C の `fopen` 等 — **遵守**。
- 置換: `find(s1, pos)` と `pos = found + s1.length()` により **左から・非重複**（例: `"aaaa"` / `"aa"` / `"b"` → `"ba"`）。

### 4.2 CLI と失敗の基本形

- `argc != 4` → Usage、終了 1。
- `s1` 空 → 終了 1。
- 入力 open 失敗 → 終了 1、**その時点では `.replace` を開かない**（読込成功後のみ書込）。

### 4.3 RAII

- `std::ifstream` / `std::ofstream` による自動クローズ — Module 01 として妥当。

---

## 5. 指摘事項

### F1（中）読み取り失敗の検知が機能していない

**場所**: `main.cpp` — `readFile` 内

```cpp
buffer << in.rdbuf();
if (!in.good() && !in.eof())
    return false;
```

**問題**: `rdbuf()` 経由の読み取り失敗は主に **ストリーム `in` の状態を更新しない**。開けた直後の `in` は `good()` のままになり、この条件は **ほぼ常に false**（実質デッドコード）。

**影響**: 例として **ディレクトリを filename に指定** した場合、open は成功し得るが読み取り内容が空 → **空の `.replace` を exit 0 で生成**し得る。

**推奨**:

- `if (in.bad()) return false;` に変更する、または
- 「読めなかったものは空として扱う」と割り切り条件を削除し、**テストと architecture で挙動を固定**する。

**注**: C++98 + iostream のみでは「空ファイル」と「ディレクトリ」を完全区別は難しい。課題スコープでは **区別しない方針を明記** するのが現実的。

---

### F2（中）書き込み途中失敗で不完全な `.replace` が残る

**場所**: `writeFile` — `out << content` 後に `good()` チェック

**問題**: ディスクフル等で **部分書き込み** 後に失敗検知しても、既にできた `.replace` は削除されない。

**設計書の失敗保証**: 「不完全ファイルを残さない」— **未実装**。

**推奨（課題スコープ）**:

- 一時ファイル + rename は `std::rename`（`<cstdio>`）と禁止 API の解釈がぶつかり得る。
- **`architecture.md` に「部分書き込みは残り得る」と明記** して割り切る、または評価環境で再現性の低いケースとしてテスト対象外とする。

---

### F3（中）テストの穴

**場所**: `tests/run_tests.sh`

**現状カバー**:

- 統合: `simple.in`、`aaaa` / `aa` / `b`
- CLI: argc 不足、存在しないファイル、空 s1

**不足（`architecture.md` の ID との差）**:

| ID | 内容 | 状態 |
|----|------|------|
| T-REP-01 | `"hello"`, `l`→`L` | 統合で間接的に近いが **単体なし** |
| T-REP-03 | s1 無し → 内容不変 | **未テスト** |
| T-CLI-05 | 入力 open 失敗時 **`.replace` 未作成** | **未テスト** |

**その他未検証（任意だが defense で聞かれ得る）**:

- `s2` 空（削除）
- `s1 == s2`
- 末尾改行の保持
- 既存 `.replace` の上書き

**推奨**: `tests/test_replace.cpp` 等で `replaceAll` 単体をリンクし、Makefile の `test` から実行。シェル側で「missing file の前後に `.replace` が無い」assert を追加。

---

### F4（小）体裁・ Makefile の後始末

- **`main.cpp`**: 行ごとに空行が入り、行数が約 2 倍 — 42 norm / 可読性のため **詰める**。
- **`Makefile` `fclean`**: `tests/out.txt` を削除するが、`run_tests.sh` の `OUT` は **未使用** — 変数・fclean 行を整理。

---

### F5（小）空 `s1` の責務が二重

- **main**: 空 s1 を拒否（契約の Pre）。
- **replaceAll**: 空 s1 で `content` をそのまま返す（無限ループ防止の防御）。

**問題**: 契約上の owner が二重に見える。

**推奨**: `replace_.hpp` / `replace.hpp` のコメントで Pre「`s1` 非空」を明記。実装側ガードは「防御的；呼び出し側が Pre を保証」と defense で説明。

---

## 6. 優先度付きアクション

| 順 | ID | 内容 |
|----|-----|------|
| 1 | F3 | テスト追加（特に T-REP-03, T-CLI-05） |
| 2 | F1 | `readFile` の失敗判定修正または挙動の明文化 |
| 3 | F4 | `main.cpp` 空行整理、Makefile / テスト脚本の dead code 削除 |
| 4 | F2 | 部分書き込みを architecture に明記 |
| 5 | F5 | ヘッダコメントで Pre を明示 |

---

## 7. Defense で説明する要点

1. **なぜクラスにしないか** — 跨る状態・不変条件がない（`class.md` § ex04 結論）。
2. **公開 API** — `replaceAll`（評価シートの replace 相当）。
3. **置換規則** — 左から、マッチ後は `pos += s1.length()`。
4. **RAII** — ストリームのデストラクタ；自前 File クラスは ex04 では不要。
5. **既知の限界** — F1/F2（ディレクトリ・部分書き込み）を正直に述べ、テスト範囲を説明。

---

## 8. 提出前チェック

- [ ] `grep` で `std::string` の `.replace(` メンバ呼び出しがない
- [ ] C ファイル API 不使用
- [ ] `make` / `make test` が Linux 環境で通る
- [ ] 提出ディレクトリから `_*` 参照実装を除く（または提出 zip に含めない）
- [ ] F3 の最低限テストを追加したか

---

## 9. 関連ドキュメント

| ファイル | 用途 |
|----------|------|
| `architecture.md` | 初学者向け設計 |
| `class.md` | クラス化判断 |
| `review.md` | 本レビュー |
