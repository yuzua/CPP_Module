# ex01 Architecture

CPP Module 00 ex01（My Awesome PhoneBook）の設計メモ。

---

## 全体像

起動すると空の電話帳ができ、ユーザーは **ADD / SEARCH / EXIT** の3コマンドを入力し続ける。

| コマンド | 動作 |
|---|---|
| `ADD` | 連絡先を1件追加 |
| `SEARCH` | 一覧表示 → index 入力 → 詳細表示 |
| `EXIT` | 終了（保存なし） |

- 最大 **8件** まで保存
- 9件目以降は **最古の連絡先を上書き**
- 動的確保（`new` / `malloc`）は禁止

---

## クラス構成

### Contact（連絡先1件）

1人分のデータを保持する。**PhoneBook が管理するオブジェクト**。

| フィールド | 型 |
|---|---|
| first name | string |
| last name | string |
| nickname | string |
| phone number | string |
| darkest secret | string |

### PhoneBook（電話帳本体）

- `Contact[8]` の固定配列を持つ
- 連絡先の追加・検索を担当
- **Contact を保持・管理する側（エンティティ / 集約）**

### main

- コマンドループ
- `EXIT` で終了（`PhoneBook::exit()` は不要。スコープ終了時に自動破棄）

```
main
 ├─ ADD    → phonebook.addContact()
 ├─ SEARCH → phonebook.searchContact()
 └─ EXIT   → break;
```

### Contact と PhoneBook の関係

```
PhoneBook（同じインスタンスが継続）
  └─ Contact[8]（値オブジェクト寄り。追加・上書きは差し替え）
```

| クラス | 性質 | 変更方法 |
|---|---|---|
| Contact | 値オブジェクト寄り | 新インスタンス作成・差し替え |
| PhoneBook | エンティティ寄り | 同じインスタンスを更新 |

---

## 9件目以降の上書き

```
1件目 ADD → [0]
8件目 ADD → [0][1][2][3][4][5][6][7]
9件目 ADD → [8][1][2][3][4][5][6][7]  ← [0] が上書き
```

PhoneBook の private に以下があると実装しやすい：

- `contactCount: int` — 現在何件入っているか（最大8）
- `oldestIndex: int` — 次に上書きする位置

---

## SEARCH の挙動

### 第1段階：一覧テーブル

4列：`index` / `first name` / `last name` / `nickname`

- 各列 **幅10文字**
- 列区切りは `|`
- **右寄せ**
- 10文字超は切り詰め、最後を `.` にする

例：

```
     index|first name| last name|  nickname|
         0|     Alice|     Smith|      Ally|
         1|      John|       Doe|        JD|
```

### 第2段階：詳細表示

index を入力させ、5項目すべてを1行ずつ表示。

---

## Contact 設計案

### 案一覧

| 案 | 内容 |
|---|---|
| **A案** | フィールドごとに getter / setter |
| **B案** | `setFields(...)` + 個別 getter |
| **C案** | `getField(Field)` / `setField(Field, value)` |
| **D案** | B案 + `printDetails()` など表示系メソッド |
| **E案** | setter なし。コンストラクタで生成し、変更は新インスタンス作成（『良いコード/悪いコード』第2版寄り） |

### A案

```cpp
void setFirstName(const std::string& value);
std::string getFirstName() const;
// ...
```

### B案

```cpp
void setFields(first, last, nick, phone, secret);
std::string getFirstName() const;
// ...
```

### C案

```cpp
enum Field { FIRST_NAME, LAST_NAME, NICKNAME, PHONE, SECRET };

void setField(Field field, const std::string& value);
std::string getField(Field field) const;
```

### D案

```cpp
void setFields(...);
std::string getFirstName() const;
// ...
void printDetails() const;
```

### E案

```cpp
Contact(const std::string& firstName,
        const std::string& lastName,
        const std::string& nickname,
        const std::string& phoneNumber,
        const std::string& darkestSecret);

std::string getFirstName() const;
// ... setter なし

void printDetails() const;  // D案要素（任意）
```

ADD 時：

```cpp
Contact contact(firstName, lastName, nickname, phoneNumber, darkestSecret);
phonebook.addContact(contact);
```

E案の思想は **値オブジェクトを不変にする** こと。貧血ドメイン対策ではなく、Contact が「中身で意味が決まる値」だから適用する。

---

## 評価指標

設計評価に使う指標：

| 種類 | 代表例 |
|---|---|
| 指標 | 凝集度（高いほどよい）、結合度（低いほどよい） |
| 原則 | SOLID、GRASP |
| パターン | Value Object、Repository/Facade |
| 実用チェック | 単一責任、カプセル化、変更箇所の明確さ |

### SOLID（ex01 で重要なもの）

| 原則 | 意味 |
|---|---|
| **S** 単一責任 | 1クラス1理由で変更 |
| **O** 開放閉鎖 | 拡張に開き、修正に閉じる |
| **I** インターフェース分離 | 不要なメソッドを公開しない |

### GRASP

| 原則 | 意味 |
|---|---|
| Information Expert | データを持つクラスがその操作を担当 |
| Low Coupling | 依存を減らす |
| High Cohesion | 1クラス1役割 |
| Controller | システム操作の入口（main） |

---

## 数値化の基準

各指標を **1〜5点** で評価する。ex01 の Contact 設計を前提とした相対評価。

| 点数 | 意味 | 旧記号との対応 |
|---|---|---|
| **5** | ex01 として非常に適している | ◎ |
| **4** | 問題なく使える | ○ |
| **3** | 許容範囲。弱点はある | △〜○ |
| **2** | やや弱い。別案の方が自然 | △ |
| **1** | ex01 には不向き | × |

### 評価項目の定義

| # | 項目 | 評価の見方 |
|---|---|---|
| 1 | 凝集度 | 1クラス1目的にまとまっているか（高いほど高得点） |
| 2 | 結合度 | 呼び出し側の依存が少ないか（低いほど高得点） |
| 3 | 単一責任 (S) | Contact の責務が明確か |
| 4 | 開放閉鎖 (O) | 項目追加・変更時に既存 API を保ちやすいか |
| 5 | インターフェース分離 (I) | 必要な操作だけ公開しているか |
| 6 | 可読性 | コードから意図が読み取れるか |
| 7 | 変更容易性 | 変更箇所が局所化されているか |
| 8 | 呼び出し側共通化 | ADD/SEARCH で重複コードが少ないか |
| 9 | 内部構造の漏れ | 項目構成を呼び出し側が知る必要が少ないか |
| 10 | 値オブジェクト適合 | Contact を「値」としてモデル化できているか |
| 11 | ex01 適合度 | 課題要件・42 評価への適合 |

### 総合スコアの算出

```
総合スコア = 全11項目の平均（小数第1位まで）
```

ex01 向けの **参考ウェイト付きスコア**（任意）：

| 項目 | ウェイト |
|---|---|
| ex01 適合度 | ×2 |
| 可読性 | ×1.5 |
| 値オブジェクト適合 | ×1.5 |
| その他 | ×1 |

---

## 各案の評価（記号）

### 1. 凝集度（高いほどよい）

| 案 | 評価 | コメント |
|---|---|---|
| A | ◎ | 1件分の Contact データ管理に集中 |
| B | ◎ | A と同等。ADD も自然 |
| C | ○ | データ保持 + 汎用アクセスが混ざる |
| D | ◎ | データ + 自分の表示責務まで持つ |
| E | ◎ | 「連絡先という値」に集中 |

### 2. 結合度（低いほどよい）

| 案 | 評価 | コメント |
|---|---|---|
| A | ◎ | 必要な getter だけ使える |
| B | ◎ | A と同様 |
| C | △ | `Field` enum に依存 |
| D | ◎ | 詳細表示は `printDetails()` だけ |
| E | ◎ | setter / enum 不要 |

### 3. SOLID

#### S: 単一責任

| 案 | 評価 |
|---|---|
| A | ◎ |
| B | ◎ |
| C | ○ |
| D | ○〜◎ |
| E | ◎ |

#### O: 開放閉鎖

| 案 | 評価 |
|---|---|
| A | △ |
| B | ○ |
| C | ◎ |
| D | ◎ |
| E | ◎ |

#### I: インターフェース分離

| 案 | 評価 |
|---|---|
| A | ◎ |
| B | ◎ |
| C | △ |
| D | ◎ |
| E | ◎ |

### 4. 可読性

| 案 | 評価 |
|---|---|
| A | ◎ |
| B | ◎ |
| C | △ |
| D | ◎ |
| E | ◎ |

### 5. 変更容易性

| 変更内容 | A | B | C | D | E |
|---|---|---|---|---|---|
| 項目追加 | getter/setter 追加 | getter + setFields 変更 | enum/switch 変更 | Contact 内だけ | コンストラクタ + getter 追加 |
| ADD 変更 | setter 5回 | setFields 1回 | setField 5回 | setFields 1回 | コンストラクタ 1回 |
| SEARCH 詳細変更 | 呼び出し側 | 呼び出し側 | 呼び出し側 | Contact 内 | Contact 内 |

### 6. 呼び出し側の共通化

| 案 | 評価 | 例 |
|---|---|---|
| A | △ | getter を項目数分書く |
| B | △ | A と同様 |
| C | ◎ | `for (Field f) getField(f)` |
| D | ◎ | `contact.printDetails()` |
| E | ○〜◎ | 生成1行 + `printDetails()` |

### 7. 内部構造の漏れ

| 案 | 評価 | 漏れ方 |
|---|---|---|
| A | △ | メソッド名として項目名が漏れる |
| B | △ | A と同様 |
| C | △ | `Field` enum として漏れる |
| D | ◎ | 詳細表示は項目構成を隠せる |
| E | ◎ | 生成後は読み取りのみ |

### 8. 値オブジェクト適合

| 案 | 評価 | コメント |
|---|---|---|
| A | △ | setter で可変 |
| B | △ | setFields で可変 |
| C | △ | setField で可変 |
| D | △ | データ自体は可変 |
| E | ◎ | 不変。差し替えモデル |

### 9. ex01 適合度

| 要件 | A | B | C | D | E |
|---|---|---|---|---|---|
| ADD | ○ | ◎ | ○ | ◎ | ◎ |
| SEARCH 一覧 | ◎ | ◎ | ○ | ◎ | ◎ |
| SEARCH 詳細 | ◎ | ◎ | ○ | ◎ | ◎ |
| 固定5項目 | ◎ | ◎ | △ | ◎ | ◎ |
| 初学者向け | ◎ | ◎ | △ | ○ | ○ |

---

## 数値化評価表

### 項目別スコア（1〜5点）

| # | 項目 | A | B | C | D | E |
|---|---|---|---|---|---|---|
| 1 | 凝集度 | 5 | 5 | 4 | 5 | 5 |
| 2 | 結合度 | 5 | 5 | 2 | 5 | 5 |
| 3 | 単一責任 (S) | 5 | 5 | 4 | 4 | 5 |
| 4 | 開放閉鎖 (O) | 2 | 4 | 5 | 5 | 5 |
| 5 | インターフェース分離 (I) | 5 | 5 | 2 | 5 | 5 |
| 6 | 可読性 | 5 | 5 | 2 | 5 | 5 |
| 7 | 変更容易性 | 4 | 5 | 4 | 5 | 5 |
| 8 | 呼び出し側共通化 | 2 | 2 | 5 | 5 | 4 |
| 9 | 内部構造の漏れ | 3 | 3 | 3 | 5 | 5 |
| 10 | 値オブジェクト適合 | 3 | 3 | 3 | 3 | 5 |
| 11 | ex01 適合度 | 4 | 5 | 2 | 5 | 5 |

### 総合スコア

| 案 | 平均（11項目） | ウェイト付き（参考） | 順位 |
|---|---|---|---|
| **E** | **4.5** | **4.9** | 1 |
| **D** | **4.3** | **4.7** | 2 |
| **B** | **4.3** | **4.3** | 3 |
| **A** | **3.9** | **4.3** | 3 |
| **C** | **3.3** | **3.1** | 5 |

ウェイト付きの算出方法（参考）：

```
加重スコア = (ex01適合×2 + 可読性×1.5 + 値オブジェクト×1.5 + その他8項目の合計) / 13
```

- 強調項目: ex01 適合度 ×2、可読性 ×1.5、値オブジェクト適合 ×1.5
- その他 8 項目: 各 ×1
- 分母: 2 + 1.5 + 1.5 + 8 = **13**

---

## 総合評価表（記号）

| 指標 | A | B | C | D | E |
|---|---|---|---|---|---|
| 凝集度 | ◎ | ◎ | ○ | ◎ | ◎ |
| 結合度 | ◎ | ◎ | △ | ◎ | ◎ |
| 単一責任 (S) | ◎ | ◎ | ○ | ○〜◎ | ◎ |
| 開放閉鎖 (O) | △ | ○ | ◎ | ◎ | ◎ |
| インターフェース分離 (I) | ◎ | ◎ | △ | ◎ | ◎ |
| 可読性 | ◎ | ◎ | △ | ◎ | ◎ |
| 変更容易性 | ○ | ◎ | ○ | ◎ | ◎ |
| 呼び出し側共通化 | △ | △ | ◎ | ◎ | ○〜◎ |
| 内部構造の漏れ | △ | △ | △ | ◎ | ◎ |
| 値オブジェクト適合 | △ | △ | △ | △ | ◎ |
| ex01 適合 | ○ | ◎ | △ | ◎ | ◎ |

---

## 各案の位置づけ

### A案

- **強み**: シンプル、OOP 的に素直
- **弱み**: ADD が冗長、呼び出し側共通化が弱い
- **スコア**: 平均 3.9

### B案

- **強み**: ex01 でバランスが良い
- **弱み**: 詳細表示で呼び出し側が項目を知る
- **スコア**: 平均 4.3

### C案

- **強み**: API 固定、呼び出し側のループ共通化
- **弱み**: enum 依存、可読性低下、ex01 には過剰
- **向く用途**: 可変項目の設定データ
- **スコア**: 平均 3.3

### D案

- **強み**: 共通化とカプセル化を両立
- **弱み**: クラスが少し太る、データは可変のまま
- **スコア**: 平均 4.3

### E案

- **強み**: 値オブジェクトとして自然、不変、『良いコード/悪いコード』第2版に合致
- **弱み**: 初学者にはコンストラクタ生成の概念がやや抽象度が高い
- **スコア**: 平均 4.5

---

## 設計上の論点

### EXIT は main で処理する

`PhoneBook::exit()` は不要。`main` で `break` し、スコープ終了時に自動破棄される。

### ポリモーフィズムと拡張性

「拡張性」と「ポリモーフィズム」は別物。

| 方式 | 内容 |
|---|---|
| C案の拡張性 | 公開 API 固定 + 内部 switch 拡張 |
| ポリモーフィズム | 型ごとに振る舞いを差し替える |
| D/E案 | 呼び出し側共通化を `printDetails()` 等で実現 |

### 値オブジェクトとエンティティ

| クラス | 種類 | 変更方法 |
|---|---|---|
| Contact | 値オブジェクト寄り | 新インスタンス作成・差し替え |
| PhoneBook | エンティティ寄り | 同じインスタンスを更新 |

E案の「setter の代わりに新インスタンス」は **Contact が値だから** であり、PhoneBook には適用しない。

### 内部構造の漏れと共通化

- 個別 getter だけだと呼び出し側共通化が弱い
- 共通化のために `getField(Field)` を公開すると enum が漏れる
- **共通化したい処理を Contact 側に移す**（D/E案）のが OOP 的に両立しやすい

---

## 採用方針

ex01 では **E案をベースに、D案の `printDetails()` を足す** のが数値評価上も最もバランスが良い。

```cpp
class Contact {
public:
    Contact(const std::string& firstName,
            const std::string& lastName,
            const std::string& nickname,
            const std::string& phoneNumber,
            const std::string& darkestSecret);

    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getNickname() const;
    std::string getPhoneNumber() const;
    std::string getDarkestSecret() const;

    void printDetails() const;
};
```

| 部分 | 案 | 用途 |
|---|---|---|
| コンストラクタ生成 | E案 | ADD（新 Contact を作成） |
| 個別 getter | E案 | SEARCH 一覧 |
| `printDetails` | D案 | SEARCH 詳細表示 |

**B案（setFields + getter）** も ex01 では十分アリ（平均 4.3）。実装を単純にしたい場合は B+D でもよい。

---

## PhoneBook 設計（概要）

```cpp
class PhoneBook {
private:
    Contact _contacts[8];
    int _contactCount;
    int _oldestIndex;

public:
    void addContact(const Contact& newContact);
    void searchContact();
};
```

| メソッド | 責務 |
|---|---|
| `addContact()` | 新しい Contact を追加（8件超えたら最古を上書き） |
| `searchContact()` | 一覧表示 → index 入力 → 詳細表示 |

SEARCH の2段階を1メソッドにまとめてもよい。より分離するなら：

```cpp
void displayContactList();
void displayContact(int index);
```

---

## ファイル構成

| ファイル | 役割 |
|---|---|
| `main.cpp` | コマンドループ |
| `Contact.hpp` / `Contact.cpp` | 連絡先クラス |
| `PhoneBook.hpp` / `PhoneBook.cpp` | 電話帳クラス |
| `Makefile` | ビルド（実行ファイル名: `phonebook`） |

---

## 参照：OOP 評価チェックリスト

1. このクラスの変更理由は1つか（単一責任）
2. 内部実装を隠せているか（カプセル化）
3. 他クラスが内部構造を知らなくてよいか（低結合）
4. 1クラスに無関係な処理が混ざっていないか（高凝集）
5. 将来変更するとき、どこを直すか明確か（変更容易性）

---

## 参照：クラス図

各案のクラス図は `ex01.dio` を参照（A案〜E案の5ページ）。
