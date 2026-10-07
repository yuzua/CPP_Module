# CPP Module 04 — サブタイプのポリモーフィズム完全理解ノート

対象課題: **C++ Module 04**（version 13.1）
テーマ: サブタイプのポリモーフィズム、抽象クラス、インターフェース

この文書は、Module 02 の「同じ名前の別関数（オーバーロード）」と
Module 03 の「継承」の上に、**実行時に呼び先が決まる仕組み**を積み上げる。

> **使い方**
> 答えの丸写し用ではない。自分で書いたコードがなぜそう動くのかを、
> ピア評価で口頭説明できる状態にするためのノートである。
> コード片は概念を目で見るための図。手で書き直すこと。
> 課題第III章のとおり、説明できないコードは評価で落ちる。
>
> 前提として `02/knowledge.md` と `03/knowledge.md` がある。
> 特に Module 03 の「構築と破棄の連鎖」「スライシング」「名前隠蔽とオーバーライドの違い」
> 「基底ポインタの `delete` はデストラクタが virtual でないと未定義動作」は、このモジュールの土台。

---

## 目次

- [0. 全体像](#0-全体像)
- [1. このモジュールに入る前に持っているもの](#1-このモジュールに入る前に持っているもの)
- [2. ポリモーフィズムは3種類ある](#2-ポリモーフィズムは3種類ある)
- [3. 静的な型と動的な型](#3-静的な型と動的な型)
- [4. virtual と vtable — 実行時に関数が選ばれる仕組み](#4-virtual-と-vtable--実行時に関数が選ばれる仕組み)
- [4.8 機械語・メモリ配置・C の関数ポインタ](#48-機械語にすると何が起きるか)
- [5. virtual デストラクタ](#5-virtual-デストラクタ)
- [6. 抽象クラスと純粋仮想関数](#6-抽象クラスと純粋仮想関数)
- [7. インターフェースという呼び方](#7-インターフェースという呼び方)
- [8. 所有権・浅いコピー・深いコピー](#8-所有権浅いコピー深いコピー)
- [9. ex00 — ポリモーフィズム](#9-ex00--ポリモーフィズム)
- [10. ex01 — Brain と深いコピー](#10-ex01--brain-と深いコピー)
- [11. ex02 — 抽象クラス](#11-ex02--抽象クラス)
- [12. ex03 — インターフェース](#12-ex03--インターフェース)
- [13. 落とし穴カタログ](#13-落とし穴カタログ)
- [14. テスト戦略](#14-テスト戦略)
- [15. ビルドとツール](#15-ビルドとツール)
- [16. 知っておくと差がつくこと](#16-知っておくと差がつくこと)
- [17. defense 想定問答](#17-defense-想定問答)
- [18. 用語集](#18-用語集)
- [19. 参考文献](#19-参考文献)
- [20. 次モジュールへの接続](#20-次モジュールへの接続)
- [21. 提出前チェックリスト](#21-提出前チェックリスト)

---

## 0. 全体像

### 0.1 何が不便で、このモジュールは何を解決するのか

動物が 2 種類いるとする。鳴き声を出したい。

```cpp
void speakAll(Dog dogs[], int dogCount, Cat cats[], int catCount) {
    for (int i = 0; i < dogCount; ++i)
        dogs[i].bark();
    for (int i = 0; i < catCount; ++i)
        cats[i].meow();
}
```

鳥を足すたびに、この関数に分岐を足す。呼び出し側が「犬か猫か」を知り続けなければならない。

Module 03 で継承は覚えた。`Animal*` に `Dog` も `Cat` も入れられる。
ただし Module 03 の関数は `virtual` ではないので、**ポインタの型が `Animal*` なら `Animal` の関数が呼ばれる**。
入れ物は共通にできたが、振る舞いは共通のままである。

Module 04 で足すのは次の一文。

> ポインタの書き方は `Animal*` のままで、呼ばれる関数は中に入っている本当の型（`Dog` や `Cat`）のものにする。

これを **サブタイプのポリモーフィズム**（subtype polymorphism）と呼ぶ。
C++ では **`virtual` 関数**で実現する。

同じ考えを最後まで押し進めると ex03 になる。
キャラクターは「氷」や「治療」という具象クラスを知らない。
「マテリアという契約」だけを知っていて、スロットに入っている実体が自分の使い方を知っている。
新しい魔法を足しても、キャラクターのコードは変えない。

### 0.2 4 つの演習で登る階段

| 演習 | 作るもの | 新しく腹落ちさせること |
|------|---------|----------------------|
| **ex00** | `Animal` / `Dog` / `Cat` と、対になる `WrongAnimal` / `WrongCat` | `virtual` があると基底ポインタ経由でも派生の関数が呼ばれる。無いと基底の関数が呼ばれる |
| **ex01** | `Brain` を `Dog` と `Cat` が `new` で持つ | 基底ポインタで `delete` するには virtual デストラクタが要る。ポインタのコピーは深いコピーにする |
| **ex02** | `Animal` をインスタンス化禁止にする | 純粋仮想関数が 1 つでもあるクラスは抽象クラス。ポインタと参照は作れる |
| **ex03** | `AMateria` / `Ice` / `Cure` / `Character` / `MateriaSource` | 中身の無い純粋仮想クラスをインターフェースとして使い、所有権を契約として言葉にする |

課題は「ex03 をやらなくてもこのモジュールには合格できる」と書いている。
ex03 は合否の必須ではなく、**ここまでの理解が一本につながる確認**である。
評価中に「新しいマテリアを足して」と頼まれることが多いので、やっておく価値は大きい。

### 0.3 本当の学習目標

| # | 目標 | 自分で確認する方法 |
|---|------|-------------------|
| 1 | 静的な型と動的な型を区別して話せる | `Animal* p = new Dog();` の 2 つの型を即答できる |
| 2 | `virtual` が無い再定義はオーバーライドではないと説明できる | `WrongCat` を `WrongAnimal*` で呼ぶとどちらが鳴くか予測できる |
| 3 | vtable を「オブジェクトが持っている関数の住所録」として説明できる | 仮想呼び出しが間接 `call` であること、C の関数ポインタ表と同じであることを言える |
| 4 | 構築中・破棄中は仮想呼び出しが「今作っているクラス」で止まる理由を言える | 基底コンストラクタから `makeSound()` すると誰が鳴くか |
| 5 | 基底ポインタの `delete` に virtual デストラクタが必要な理由を、未定義動作という言葉で言える | `~Dog` が走らないと、犬のブロックとは別の `Brain` の確保が残ると言える |
| 6 | 抽象クラスと「コンストラクタを protected にしただけのクラス」の違いを言える | `new Animal()` がコンパイルエラーになる条件 |
| 7 | インターフェースは「データも実装も持たない純粋仮想クラス」という C++ の慣習だと説明できる | `ICharacter` と `AMateria` の違い |
| 8 | `new` したメモリの所有者を常に 1 人に決められる | equip / unequip / learn / create のあと、誰が `delete` するか即答できる |
| 9 | 浅いコピーが二重 `delete` になる道筋を図に描ける | `Dog` のコピー後にアイデアを変えても、元が変わらないテストを書ける |

### 0.4 課題文に書いていないが、例から読む要件

課題自身が「例には、指示に明示されていない要件が含まれる」と書いている。ここが本番。

| 見た目 | そこから読む要件 |
|--------|------------------|
| `const Animal* j = new Dog();` のあと `j->makeSound()` | `makeSound()` は **const メンバ関数**。const でないとこの例がコンパイルできない |
| 同じ行の `j->getType()` | `getType()` も **const** |
| `i->makeSound()` が猫の鳴き声になる | `makeSound()` は **virtual** |
| ex01 の `delete j;`（`j` の型は `const Animal*`） | デストラクタは **virtual**。そうでないと未定義動作 |
| `Dog` と `Cat` が `new Brain()` する | コピーはポインタのコピーでは足りない。**深いコピー** |
| ex02「インスタンス化できない」かつ章題が抽象クラス | 純粋仮想関数で抽象クラスにする。protected コンストラクタだけではない |
| ex03 の `learnMateria(new Ice())` がポインタを保存しない | 渡した `new` の **delete 責任が MateriaSource に移る** |
| `tmp = createMateria(...); me->equip(tmp);` のあと `tmp` を unequip するテストが書ける | **equip は同じポインタを格納する**。中で別 clone すると、unequip 後に手元のポインタを delete する契約が壊れる |
| `./a.out \| cat -e` の期待出力が 2 行だけ | ex03 のクラスはコンストラクタで印字しない。`$` は cat が表示する改行の印で、プログラムは出さない |
| 全モジュール共通の規則 | OCF、ヘッダに実装を書かない、インクルードガード、`using namespace` 禁止、`friend` 禁止、STL コンテナ禁止、`printf` / `malloc` / `free` 禁止、`-std=c++98` |

### 0.5 `virtual` という単語は2つの別物に使われる

ここを混ぜると Module 03 と 04 が同時に崩れる。

| 書き方 | 意味 | どのモジュール |
|--------|------|----------------|
| `virtual void makeSound() const;` | この関数は実行時に実体の型で呼び分ける | **04** |
| `class Dog : virtual public Animal` | 仮想継承。ダイヤモンドで基底を 1 つに共有する | **03**（`DiamondTrap`） |

同じキーワードで、仕組みも目的も違う。このモジュールで付ける `virtual` は **関数**につける方だけ。

---

## 1. このモジュールに入る前に持っているもの

長く再説明はしない。この 6 つが曖昧なら、先に `03/knowledge.md` の第2章・第4章・第5章を読み直す。

1. **is-a**: `Dog` は `Animal` の一種なので、`Animal* p = new Dog();` が合法。
2. **構築は基底が先、破棄は派生が先**。`delete` が正しく仮想的なら、メッセージは `~Dog` のあと `~Animal`。
3. **初期化子リストの実行順は書いた順ではない**。基底、次にメンバの宣言順。`-Wall` の `-Wreorder` がずれると、`-Werror` でコンパイルが止まる。
4. **スライシング**: `Animal a = dog;` は `Dog` 部分を切り捨てて `Animal` の値を作る。ポインタと参照はスライスしない。
5. **非 virtual 関数の再定義は名前隠蔽**。呼ばれる関数は変数に書いた型でコンパイル時に決まる。
6. **OCF**（正統派カノニカルフォーム）: デフォルト構築、コピー構築、コピー代入、破棄。Module 02 から 09 は、課題が別形を明示したクラス以外、この 4 つを書く。

追加で、このモジュールから毎日使う言葉を先に固定する。

| 言葉 | 意味 |
|------|------|
| **静的な型** | ソースコードに書いてある型。`Animal* p` の静的な型は `Animal*` |
| **動的な型** | 実行時にそこにあるオブジェクトの本当の型。`p` が `new Dog()` を指すなら動的な型は `Dog` |
| **静的束縛** | 呼び先がコンパイル時に決まる |
| **動的束縛** | 呼び先が実行時に決まる |
| **オーバーライド** | 基底の virtual 関数を、派生クラスで同じシグネチャで置き換えること |
| **所有権** | その `new` を `delete` する責任が誰にあるか |

---

## 2. ポリモーフィズムは3種類ある

ポリモーフィズムは「1 つの名前が、複数の型で意味を持つ」という広い言葉。C++ では実現方法が 3 つあり、モジュールが分かれている。

| 種類 | 日常語 | 仕組み | いつ決まる | モジュール |
|------|--------|--------|------------|------------|
| **アドホック** | 同じ名前の別関数 | オーバーロード | コンパイル時 | 02（`Fixed` の `+` など） |
| **サブタイプ** | 基底のふりをした派生 | `virtual` | **実行時** | **04** |
| **パラメトリック** | 型を穴にした鋳型 | テンプレート | コンパイル時 | 07 |

ex00 の `makeSound()` はサブタイプ。`Dog::makeSound` と `Cat::makeSound` はオーバーロードではない。引数が同じで、クラスが違う。選ぶ基準は引数ではなく、**オブジェクトの動的な型**。

テンプレートでも「犬と猫を同じ関数に入れる」ことは後からできる。Module 04 が先に教えるのは、**型の名前を呼び出し側が書かなくても、オブジェクト自身が振る舞いを持っている**という設計。ex03 の `Character` が `Ice` という名前をソースコードに書かないのは、この性質を使うため。

---

## 3. 静的な型と動的な型

### 3.1 1 つのオブジェクトを、2 つの型で見る

```cpp
Dog *real = new Dog();
Animal *asAnimal = real;
```

メモリ上のオブジェクトは 1 匹の `Dog`。見方が 2 つある。

```text
  asAnimal の静的な型 = Animal*
  asAnimal の動的な型 = Dog

  ┌─ Animal* という窓 ─────────────┐
  │  vptr                          │
  │  type = "Dog"                  │
  │  brain ──► Brain               │  ← 窓のラベルが Animal でも、
  └────────────────────────────────┘     実体は Dog のまま
```

ポインタを `Animal*` に入れても、オブジェクトは削られない。削るのは値へのコピー（スライシング）だけ。

この区別から、次の 2 つが同時に成り立つ。

- **データ**は常に実体のもの。`type` を `Dog` のコンストラクタが `"Dog"` にしたなら、`Animal*` 経由の `getType()` でも `"Dog"` が返る。`getType()` が virtual でなくても、である。基底の関数本体が、派生が書いたメンバを読んでいるだけだから。
- **関数本体**は、virtual かどうかで選ばれ方が変わる。virtual なら動的な型の本体。非 virtual なら静的な型の本体。

ex00 の課題 main が `getType()` で `"Dog"` / `"Cat"` を出し、`makeSound()` で種類ごとの鳴き声を出す、という対比はここから来ている。

| 関数 | virtual が要るか | 理由 |
|------|------------------|------|
| `getType()` | 無くて動く | 違いはデータ `type` に入っている。基底の関数がそれを返すだけで足りる |
| `makeSound()` | **要る** | 違いは関数の中身にある。データには鳴き声の処理は入っていない |

`getType()` を virtual にしても動く。課題の要件は満たせる。ただし「virtual にしないと動かない関数」は `makeSound()` の方だと説明できること。

### 3.2 呼び出しの 4 パターン

`Dog` が `Animal::makeSound` をオーバーライドしているとして、

```cpp
Dog dog;
Animal *p = &dog;
Animal &r = dog;
Animal copy = dog;   // ex02 で Animal が抽象になると、この行はコンパイルできない
```

| 呼び出し | 静的な型 | 動的な型 | virtual のとき | 非 virtual のとき |
|----------|----------|----------|----------------|-------------------|
| `dog.makeSound()` | `Dog` | `Dog` | `Dog` | `Dog` |
| `p->makeSound()` | `Animal` | `Dog` | **`Dog`** | **`Animal`** |
| `r.makeSound()` | `Animal` | `Dog` | **`Dog`** | **`Animal`** |
| `copy.makeSound()` | `Animal` | `Animal`（スライス済み） | `Animal` | `Animal` |

1 行目は virtual がなくても `Dog` が呼ばれる。**自分のテストが具体型だけで `dog.makeSound()` していると、virtual を付け忘れても正しく見えてしまう。** 課題の例は必ず基底ポインタ経由である。テストもそう書く。

最後の行は、コピーした瞬間に動的な型が `Animal` になる。virtual でも派生の関数には届かない。届けるにはポインタか参照で扱う。配列に入れるなら `Animal*` の配列であり、`Animal` の配列ではない。参照の配列は C++ では作れない。

---

## 4. virtual と vtable — 実行時に関数が選ばれる仕組み

### 4.1 規格が保証すること、コンパイラがやっていること

C++ の規格は「virtual 関数は、呼びに使ったオブジェクトの動的な型の最終オーバーライドが呼ばれる」と決めている。**vtable という語は規格に出てこない。** 学校の gcc / clang は Itanium C++ ABI という配置で、次のモデルを使ってその保証を実装している。理解の道具としてこのモデルで考えてよい。

### 4.2 たとえ（関数の欄だけを数えた図）

各クラスは **関数の住所録（vtable）** を 1 冊だけ持つ。オブジェクトごとではない。
各オブジェクトは、自分のクラスの住所録を指す **隠しポインタ（vptr）** を 1 本持つ。

下の番号は「仮想関数を宣言順に数えた欄」である。Itanium ABI の実アドレスではない。
実物は vptr の手前にオフセットと型情報があり、デストラクタは 2 枠を使う。そこは [4.11](#411-itanium-abi-では-vptr-が表の途中を指す)。

```text
  Dog オブジェクト                 Dog の vtable（クラスに 1 つ）
  ┌──────────────────┐            ┌────────────────────────────┐
  │ vptr ────────────┼──────────► │ [0] Dog のデストラクタ      │
  │ type = "Dog"     │            │ [1] Dog::makeSound          │
  │ brain            │            └────────────────────────────┘
  └──────────────────┘

  Cat オブジェクト                 Cat の vtable
  ┌──────────────────┐            ┌────────────────────────────┐
  │ vptr ────────────┼──────────► │ [0] Cat のデストラクタ      │
  │ type = "Cat"     │            │ [1] Cat::makeSound          │
  └──────────────────┘            └────────────────────────────┘

  Animal の vtable
  ┌────────────────────────────┐
  │ [0] Animal のデストラクタ   │
  │ [1] Animal::makeSound       │
  └────────────────────────────┘
```

`Animal* p = new Dog(); p->makeSound();` のとき、コンパイラは次のコードを出す。

```text
  1. p が指すオブジェクトの先頭から vptr を読む
  2. その vtable の「makeSound の欄」に入っているアドレスを読む
  3. そこへ飛ぶ
```

`p` の静的な型は、コンパイル時に「`makeSound` という欄が存在すること」と「const で呼べること」を確認するために使う。**どのアドレスに飛ぶかは実行時の vptr が決める。**

非 virtual の `WrongAnimal::makeSound` にはこの欄が無い。コンパイラは `WrongAnimal::makeSound` のアドレスを、呼び出し命令に直接書き込む。実行時に中身を見に行かない。

### 4.3 コスト

| 項目 | 内容 |
|------|------|
| サイズ | オブジェクトごとにポインタ 1 本（64bit なら 8 バイト）増える。空のクラスは本来 1 バイトだが、virtual 関数があるとポインタサイズ以上になる |
| 呼び出し | 直接呼び出しが、メモリを 2 回読む間接呼び出しになる |
| インライン化 | 動的な型がコンパイル時に分からない呼び出しは、インライン化しにくい |
| 住所録自体 | クラスごとに 1 つ、読み取り専用データとして共有される。犬が 100 匹いても vtable は 1 冊 |

この課題の規模では、評価で「virtual にすると何が増えるか」と聞かれたときの答えは上の表で足りる。
命令の形、分岐予測、プロセスのどの領域に表が置かれるかは [4.8](#48-機械語にすると何が起きるか) 以降。

### 4.4 vptr はコンストラクタが書き換える

`Dog` を構築する間、vptr は途中で差し替わる。

```text
  1. Animal のコンストラクタが走っている間
       vptr は Animal の vtable を指す
       この中で makeSound() すると Animal::makeSound が呼ばれる

  2. Animal のコンストラクタが終わり、Dog のコンストラクタに入る
       vptr は Dog の vtable に差し替わる
       この中で makeSound() すると Dog::makeSound が呼ばれる

  3. 構築完了後
       vptr は Dog の vtable のまま
```

破棄は逆。

```text
  1. ~Dog の本体が走っている間は vptr は Dog
  2. ~Animal の本体に入ると vptr は Animal に戻る
```

**理由**: `Animal` のコンストラクタが走っている時点では、`Dog` 固有のメンバはまだ初期化されていない。ここで `Dog::makeSound` が `brain` を触ると、未初期化のポインタを辿ることになる。言語は「構築中・破棄中の動的な型は、今コンストラクタ／デストラクタを実行しているクラスである」と定め、その事故を構造的に防いでいる（C++98 [class.cdtor]）。

だから次は書かない。

```cpp
Animal::Animal() {
    this->makeSound();   // 常に Animal 版。Dog を new していても Dog 版にはならない
}
```

派生クラスのメンバを触る virtual 関数を基底コンストラクタから呼ぶと、未初期化アクセスになる。純粋仮想関数のまま解決されてしまう呼び出しは未定義動作で、実行時に "pure virtual method called" で落ちることがある。

### 4.5 シグネチャが 1 文字でも違うと、オーバーライドにならない

C++98 には `override` キーワードが無い（C++11。この課程では禁止）。コンパイラは「書き間違い」と「別関数を足した」を区別しない。

課題の例は `const Animal*` なので、基底はこうなる。

```cpp
virtual void makeSound() const;
```

派生で const を落とすと、**別の関数**になる。

```cpp
class Dog : public Animal {
    public:
        void makeSound();              // const が無い。オーバーライドしていない
};
```

`const Animal* p = new Dog(); p->makeSound();` はコンパイルできる。const オブジェクトから呼べるのは const 版だけで、それは `Animal::makeSound` しか無い。犬は鳴かない。警告も、`-Wall -Wextra` だけでは出ないことがある。

ローカル確認では次を足すと、この事故を教えてくれる。提出用 Makefile には入れなくてよい。

```text
-Woverloaded-virtual -Wnon-virtual-dtor
```

`-Woverloaded-virtual` は「基底の virtual を隠しそうな派生の関数」を警告する。`-Wnon-virtual-dtor` は「public なデストラクタが virtual でない多態的基底」を警告する。どちらも `-Wall` には含まれない。

派生側に `virtual` を重ねて書く必要は無い。基底で virtual なら、オーバーライドも virtual のまま。書いておくと読み手には親切。

### 4.6 アクセスチェックは静的な型で行う

virtual でも、呼べるかどうかはポインタの型でコンパイル時に決まる。`Animal` の public な `makeSound` は `Animal*` から呼べる。派生だけが public で基底が private、という配置にすると `Animal*` からは呼べない。この課題では `makeSound` は基底の public でよい。

### 4.7 デフォルト引数は静的な型で決まる

virtual 関数のデフォルト引数は、動的な型ではなく静的な型の宣言が使われる。基底と派生でデフォルト引数を変えると、見た目と違う値が渡る。この課題ではデフォルト引数を使わない、で回避できる。知っておくと「virtual なのに一部だけ静的」という例外として説明できる。

### 4.8 機械語にすると何が起きるか

学校の Linux（x86-64）は System V AMD64 ABI である。メンバ関数の `this` は、第 1 引数と同じく **`rdi`** に入る。

`Dog dog; dog.makeSound();` は静的な型が `Dog` なので、コンパイラは飛び先を命令の中に書き込む。**直接呼び出し**である。

```text
  lea    dog(%rip), %rdi      # this = &dog
  call   Dog::makeSound       # 飛び先が命令に書いてある
```

`Animal *p = new Dog(); p->makeSound();` は、`makeSound` が virtual なので **間接呼び出し**になる。`p` も `rdi` に入ったあと、オブジェクトの先頭から vptr を読む。

```text
  mov    (%rdi), %rax         # rax = *this          … vptr を読む（データ）
  mov    SLOT(%rax), %rax     # rax = vtable[欄]     … 関数アドレスを読む（データ）
  call   *%rax                # そのアドレスへ飛ぶ   … 間接 call
```

`SLOT` はバイト変位である。4.2 の「欄番号 × ポインタサイズ」ではない。実物は [4.11](#411-itanium-abi-では-vptr-が表の途中を指す) の配置で決まる。宣言を足すと変位が変わる。固定の即値として暗記しない。自分の翻訳結果で見る。

```text
  c++ -std=c++98 -Wall -Wextra -Werror -S -O0 main.cpp
```

`-S` はアセンブリを出す。最適化を切ると、上の 3 命令が残りやすい。`-O2` では、コンパイラが動的な型を証明できた呼び出しを直接 `call` に戻すことがある（脱仮想化）。課題の `Animal*` 経由は、通常その証明ができない。

非 virtual の `WrongAnimal::makeSound` には `mov (%rdi)` が無い。`call WrongAnimal::makeSound` だけである。

間接 `call` が直接 `call` より高い理由は、メモリを 2 回読むことだけではない。

| | 直接 `call` | 間接 `call *%rax` |
|--|-------------|-------------------|
| 飛び先 | 命令の即値。フェッチの時点で次の命令列が分かる | 実行してレジスタを読むまで分からない |
| 分岐予測 | 戻りアドレスの予測が安定しやすい | 間接分岐予測器が履歴から当てる。犬と猫が交互だと外れやすい |
| 命令キャッシュ | 呼び先を先に読みにいける | 外れたとき、パイプラインが読んだ命令を捨てる |

犬が 100 匹いても vtable は 1 冊、というのはデータの話である。呼び出しのたびに予測が外れるかもしれない、というのが CPU の話である。この課題の匹数では計測できない。聞かれたときに「サイズ」と「間接分岐」を分けて答えればよい。

### 4.9 プロセスのどこに何が置かれるか

`new Dog()` と `Dog d;` は、どちらも同じ vtable を指す。違うのはオブジェクト本体の置き場である。

```text
  高いアドレス
    スタック     Dog d; の本体。関数を抜けると寿命が終わる。delete しない
    ヒープ       new が確保した塊。malloc の塊ヘッダがユーザ領域の直前にある
    .data/.bss   初期化済み／未初期化のグローバル
    .rodata      vtable、typeinfo、文字列リテラル。書き込み不可
    .text        Dog::makeSound の機械語
  低いアドレス
```

学校の gcc では、だいたい次の対応になる。

| C++ の操作 | ライブラリ | OS が見るもの |
|------------|------------|----------------|
| `operator new(n)` | `malloc` | 小さい塊は `brk` で伸ばしたヒープ、大きい塊は `mmap` |
| コンストラクタ | コンパイラが出した関数 | 確保済み領域の先頭に vptr を書く。OS は関与しない |
| `operator delete(p)` | `free` | 塊ヘッダを見て、その領域を再利用可能にする |
| vtable の中身 | リンカが `.rodata` に置く | 実行ファイルの読み取り専用ページ。同じバイナリのプロセス間で共有できる |

ASLR により、これらのベースアドレスは起動のたびにずれる。vtable 内の関数アドレスは、ローダが再配置する。オブジェクトの vptr には、再配置後の絶対アドレスが入る。

`.rodata` は書けない。攻撃やバグで差し替わるのは表そのものではなく、**オブジェクトの中の vptr** である。次の仮想呼び出しは、そこが指す先へ飛ぶ。制御フロー完全性（CFI）が仮想呼び出しを検査するのは、この飛び先がデータ経由だからである。課題のコードでやることは、自分で vptr を書き換えない、に尽きる。

### 4.10 C で書くと、同じ構造になる

C++ の `virtual` は新しい CPU 命令ではない。C で書いてきた「構造体に関数ポインタを並べ、先頭にその表へのポインタを持つ」を、コンパイラが生成する。

```c
struct Animal;

struct AnimalVtable {
    void (*make_sound)(struct Animal *self);
};

struct Animal {
    const struct AnimalVtable *vptr;
    const char *type;
};

struct Dog {
    struct Animal base;          /* 先頭に基底。単一継承と同じ */
    struct Brain *brain;
};

void dog_make_sound(struct Animal *self);

static const struct AnimalVtable dog_vtable = {
    dog_make_sound               /* .rodata に置かれる定数の表 */
};

void dog_init(struct Dog *dog) {
    dog->base.vptr = &dog_vtable;    /* コンストラクタが vptr を書くのと同じ */
    dog->base.type = "Dog";
    dog->brain = brain_new();
}

void speak(struct Animal *animal) {
    animal->vptr->make_sound(animal);    /* p->makeSound() と同じ間接呼び出し */
}
```

`dog_init` を呼び忘れたオブジェクトの `vptr` は不定である。C++ ではコンストラクタが書き込むので、構築が終わったオブジェクトではその穴が無い。構築の途中で表が基底のものになる、という 4.4 の規則は、この代入が「今構築しているクラスの表」に対して行われることから来ている。

Linux カーネルに C++ のクラスは無い。ファイルシステムは同じ形で分岐している。

```c
struct file_operations {
    ssize_t (*read)(struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *);
};
```

VFS は `file->f_op->read(...)` を呼ぶ。ext4 も procfs も、自分の `file_operations` を渡す。VFS の本体は ext4 の `read` を include しない。ex03 で `Character.cpp` が `Ice.hpp` を include しないのは、これと同じ依存の向きである。具象は表を埋め、呼び出し側は表の欄だけを知る。

### 4.11 Itanium ABI では、vptr が表の途中を指す

4.2 の図は関数欄だけを数えている。gcc / clang が使う Itanium C++ ABI では、**vptr は最初の仮想関数を指す**。その手前に、関数ではない 2 語がある。

```text
  アドレスの低い方
  ┌─────────────────────────┐
  │ offset-to-top           │  ← vptr から見て -2 語。この部分オブジェクトから
  │                         │     完全オブジェクト先頭までの変位
  ├─────────────────────────┤
  │ typeinfo へのポインタ    │  ← -1 語。RTTI。dynamic_cast / typeid が使う
  ├─────────────────────────┤
  │ 最初の仮想関数           │  ← vptr が指す場所。ここを 0 語目と数える
  │ 次の仮想関数             │
  │ ...                     │
  └─────────────────────────┘
  アドレスの高い方
```

仮想デストラクタは、関数欄を **2 つ**使うことが多い。

| 欄 | 名前 | 誰が呼ぶか |
|----|------|------------|
| 完全デストラクタ | complete object destructor | 派生のデストラクタが基底を破棄するとき。メモリは解放しない |
| 削除用デストラクタ | deleting destructor | `delete` 式。完全な破棄のあと `operator delete` を呼ぶ |

`delete p` が virtual に解決されるとき、飛ぶ先は削除用デストラクタである。`makeSound` の変位は、デストラクタ 2 枠のあとになることが多い。4.2 でデストラクタを `[0]`、`makeSound` を `[1]` と描いたのは、この 2 枠と手前の 2 語を畳んだ説明用である。

確認はコンパイラに聞く。

```text
  c++ -std=c++98 -fdump-class-hierarchy -c Animal.cpp
```

出力に `vtable for Dog` と、各欄の関数名が出る。変位の暗記より、このダンプを読めることの方が正確である。

### 4.12 基底が先頭に無いとき、`this` をずらしてから飛ぶ

このモジュールの `Dog : public Animal` は単一継承で、`Animal` 部分はオブジェクトの先頭にある。`Animal*` と `Dog*` のビットパターンは同じである。4.8 の `rdi` をずらさずに `Dog::makeSound` へ入れてよい。

基底が先頭に無いのは、多重継承で 2 番目以降の基底である。Module 03 の仮想継承も、基底の位置が固定ではない。

```text
  C : public A, public B の一例

  先頭
  ┌──────────┬───────┬──────────┬───────┐
  │ A の vptr │ A のメンバ │ B の vptr │ B のメンバ │
  └──────────┴───────┴──────────┴───────┘
  ▲ C* と A* はここ          ▲ B* はここ。malloc が返したアドレスではない
```

`B*` 経由で `C` がオーバーライドした関数を呼ぶとき、関数本体は `C*` を前提にメンバを読む。ABI は、B 側の vtable に **サンク**（thunk）を置く。サンクは `this` から先頭までの変位を引き、それから `C` の関数へ飛ぶ。

```text
  B 側の vtable にあるサンクの形（疑似。実命令は lea や add になる）
      rdi を、B 部分から完全オブジェクト先頭までの変位だけ戻す
      jmp  C::関数
```

`delete` も同じ調整が要る。削除用デストラクタは、`operator delete` に渡す前に完全オブジェクトの先頭へ戻す。戻さないと、`free` は malloc が返したアドレスではない内部ポインタを受け取る。ヒープの塊ヘッダはユーザ領域の直前にあるので、内部ポインタの `free` は塊を壊す。

このモジュールの単一継承では、その調整は 0 である。ヒープ破壊の話を「virtual で無いデストラクタはサイズが違うから壊れる」とまとめると、ここと混ざる。分け方は第 5 章。

---

## 5. virtual デストラクタ

### 5.1 何が問題か

```cpp
Animal *p = new Dog();
delete p;
```

`Dog` は `Brain*` を `new` している。`~Dog` がその `delete` を行う。
`~Animal` が virtual でないと、この `delete p` は **未定義動作**（C++98 [expr.delete]）。

規格の条件は次のとおり。

> `delete` するポインタの静的な型と、オブジェクトの動的な型が違うとき、静的な型は動的な型の基底であり、かつ静的な型が virtual デストラクタを持たなければならない。そうでなければ振る舞い未定義。

未定義動作は「派生のデストラクタが呼ばれない」ことすら保証しない。観測される症状と、ヒープが壊れる条件は別である。

`new` と `delete` は、確保と破棄を別々に行う。

```text
  new Dog()
      operator new(sizeof(Dog))     → malloc。塊ヘッダ + 犬の領域。戻り値を p0 とする
      Animal のコンストラクタ        → この間だけ、先頭の vptr は Animal の vtable
      Dog のコンストラクタ開始       → 先頭の vptr を Dog の vtable に差し替える
      new Brain()                    → メンバ初期化。別の malloc。犬の中の brain がそのアドレスを持つ
      Dog のコンストラクタ本体

  delete p   （p の静的な型は Animal*、デストラクタは virtual）
      vptr 経由で Dog の削除用デストラクタへ飛ぶ
          ~Dog の本体
              delete brain           → Brain の塊を free
          ~Animal の本体
          operator delete(先頭)      → free(p0)。犬の塊を free
```

デストラクタが virtual でないとき、コンパイラは `Animal` の削除用デストラクタを直接呼ぶ。`~Dog` は走らない。

| 塊 | virtual で無い `delete` のあと、C++98 + glibc で起きやすいこと |
|----|------------------------------------------------------------------|
| 犬の塊（`malloc` が `p0` として返したもの） | `free(p0)` は呼ばれることが多い。`free` は利用者からサイズを受け取らない。塊ヘッダに書いてある大きさを見る。単一継承で `Animal` が先頭なら、`Animal*` のビットパターンは `p0` と同じである |
| `Brain` の塊 | `~Dog` が走らないので `delete brain` が無い。**残るのはこちら** |

C++98 の `operator delete` は `void operator delete(void*)` だけである。サイズを受け取る形は C++14 で入った。だから「`operator delete` に `sizeof(Animal)` が渡ってヒープが壊れる」はこの課程の処理系の説明としては当たらない。

未定義動作であることは変わらない。派生オブジェクトの寿命が、派生デストラクタ無しで終わらされている。`free` が犬の塊を返したように見えても、正しいプログラムではない。

ヒープの塊を壊すのは、別の条件である。`free` に渡るポインタが `malloc` の戻り値ではないとき。基底部分がオブジェクト先頭に無い（[4.12](#412-基底が先頭に無いときthis-をずらしてから飛ぶ)）のに、削除用デストラクタが先頭へ戻さずに `free` すると、塊ヘッダではない場所を解放しようとする。このモジュールの `class Dog : public Animal` は先頭が `Animal` なので、その経路には乗らない。乗らないことと、未定義動作ではないことは別である。

ex01 の課題文は「犬や猫を直接 Animal として削除する」「適切なデストラクタが期待順で呼ばれる」「リークを作らない」と書いている。この 3 つを同時に満たすのが `virtual ~Animal()`。見るべきリークは `Brain` の塊である。

ex00 の提示 main は `delete` していない。こちらが提出するテストで `delete` するなら、ex00 の時点で virtual デストラクタを付ける。ex01 で急に思い出すより、多態的に使う基底は最初から付ける、が安全。

### 5.2 期待される順序

`delete` が `Animal*` 経由で、デストラクタが virtual で、`Dog` が `Brain` を持っているとき。

```text
  ~Dog の本体
      「Dog が破棄された」というメッセージ
      delete brain
          ~Brain の本体
  Dog のメンバが破棄される（ポインタ変数そのもの。先に delete 済みなら二重解放しない）
  ~Animal の本体
      「Animal が破棄された」というメッセージ
  Animal のメンバが破棄される（std::string type）
```

ポインタ型のメンバは、メンバとしての破棄時に **指先を delete しない**。`int*` を捨てても `int` は残るのと同じ。`delete brain` は `~Dog` の本体に自分で書く。

配列を先頭から `delete` するなら、個体の順序はループの順序。各個体の中では常に「派生のデストラクタ、その中で Brain、そのあと基底」。課題の「期待される順序」は、この連鎖を指している。

### 5.3 指針

Herb Sutter のガイドライン（*C++ Coding Standards* Item 50）:

> 多態的な基底のデストラクタは **public かつ virtual** か、**protected かつ非 virtual** のどちらか。

| 選択 | 意味 |
|------|------|
| public virtual | `Animal*` で `delete` してよい。今回はこちら |
| protected 非 virtual | 基底ポインタでの `delete` がコンパイルできなくなる。多態的削除を禁止する設計 |

`WrongAnimal` もテストで基底ポインタから `delete` するなら、デストラクタは virtual にする。Wrong 実験で非 virtual にするのは **`makeSound`**。デストラクタまで非 virtual にすると、実験の途中で未定義動作になる。見せたい違いは鳴き声の結び付き。

### 5.4 const ポインタの delete

課題の例は `const Animal* j = new Dog(); delete j;` である。const オブジェクトへのポインタでも `delete` は合法。デストラクタは const 関数ではないが、`delete` はこの形を特別に許している。

---

## 6. 抽象クラスと純粋仮想関数

### 6.1 書き方

```cpp
virtual void makeSound() const = 0;
```

`= 0` は「このクラスでは実装を提供しない。派生が実装する」という印。これを **純粋仮想関数**（pure virtual function）と呼ぶ。数値の 0 を代入しているわけではない。

**純粋仮想関数を 1 つでも持つクラスは抽象クラス**になり、オブジェクトを作れない。

```cpp
Animal a;            // コンパイルエラー
new Animal();        // コンパイルエラー
Animal *p;           // OK。ポインタはオブジェクトではない
Animal *p = new Dog(); // OK。作っているのは Dog
Animal &r = dog;     // OK
void f(Animal a);    // 引数の型として値渡しは不可（抽象クラスのオブジェクトが必要になる）
void f(Animal &a);   // OK
```

ex02 が欲しいのはこの状態。`Animal` という「鳴き声の中身が無い概念」を、実体として作れなくする。

### 6.2 protected コンストラクタとの違い

コンストラクタを protected にすると、外からは `Animal a;` も `new Animal()` もできなくなる。見た目の目的は近い。ただし **それは抽象クラスではない**。

| | 純粋仮想関数がある | コンストラクタが protected なだけ |
|--|--------------------|-----------------------------------|
| 規格上の分類 | 抽象クラス | 普通の具象クラス |
| 外からの生成 | 不可 | 不可 |
| メンバ関数の中からの生成 | 不可 | できてしまう |
| 派生が実装を強要されるか | される。未実装なら派生も抽象のまま | されない |
| 値スライス `Animal a = dog` | コンパイルできない | できてしまう |

章のタイトルが「抽象クラス」なので、手段は純粋仮想関数。自然な対象は `makeSound()`。意味のあるデフォルトの鳴き声が無い、というのが課題の理由づけ（「Animal は鳴き声を発しない」）。

デストラクタを純粋仮想にする方法でもクラスは抽象になる。その場合でもデストラクタの本体は必ず cpp に書く。派生のデストラクタが基底デストラクタを呼ぶから。今回のメッセージ表示は、純粋ではない virtual デストラクタの本体に置けば足りる。`makeSound() = 0` の方を採用する。

### 6.3 派生クラス側

```cpp
class Dog : public Animal {
    public:
        void makeSound() const;    // これを定義すれば Dog は具象クラス
};
```

`Dog` が `makeSound` を実装し忘れると、`Dog` も抽象クラスのままになり、`new Dog()` がコンパイルエラーになる。エラーメッセージは "cannot instantiate abstract class" と、未実装の純粋仮想関数の名前。

`Dog` のさらに派生を作る場合、`makeSound` はすでに virtual なので、再オーバーライドできる。

### 6.4 純粋仮想関数にも本体を書ける

`= 0` と、cpp 側の定義は両立する。クラスは抽象のまま。派生が `Animal::makeSound()` と明示したときだけ、その本体が使える。仮想呼び出しが純粋仮想のまま解決されるのは未定義動作。この課題では本体を書かなくてよい。リンクは、誰もそのシンボルを呼ばなければ通る。

### 6.5 抽象クラスのコンストラクタは必要

抽象クラスは自分ではインスタンス化できないが、派生の中の「基底部分」としては構築される。デフォルトコンストラクタ、`type` を受け取るコンストラクタ、コピー、代入、virtual デストラクタは、OCF と課題のメッセージ要件のために書く。`new Animal()` がエラーになることと、`Dog` の初期化子リストから `Animal()` を呼ぶことは両立する。

### 6.6 名前を `AAnimal` にしてよいか

課題は「必要なら `A` を前置してよい」と書いている。42 では抽象クラスに `A` を付ける慣習があり、ex03 の `AMateria` がその例。

| 選択 | 結果 |
|------|------|
| `Animal` のまま | ファイル名も `Animal.hpp`。ex00 からの連続として読みやすい |
| `AAnimal` に変える | ファイル名も `AAnimal.hpp` / `AAnimal.cpp` に変える。クラス名とファイル名をずらさない |

どちらも仕様上は正しい。中途半端にクラスだけ改名してファイル名を残すと、命名規則（ファイル名はクラス名）に反する。

ex02 のディレクトリは、ex01 のファイル一式のコピーを改変したもの。ex00 のディレクトリを後から書き換えない。各演習は単体で `make` できる。

---

## 7. インターフェースという呼び方

### 7.1 C++ に interface キーワードは無い

課題が明記しているとおり、C++98 にも C++20 にも Java の `interface` は無い。慣習として次をインターフェースと呼ぶ。

> データメンバを持たず、関数がすべて純粋仮想（デストラクタだけは本体を持つ）のクラス。

```cpp
class ICharacter {
    public:
        virtual ~ICharacter() {}
        virtual std::string const & getName() const = 0;
        virtual void equip(AMateria* m) = 0;
        virtual void unequip(int idx) = 0;
        virtual void use(int idx, ICharacter& target) = 0;
};
```

デストラクタに本体がある理由は、純粋仮想でなくても、基底を `delete` するには virtual デストラクタの定義が要るから。課題がクラス定義の中に `{}` を書いている。この 2 つのインターフェースに限っては、課題の掲載どおりヘッダに空の本体を置いてよい。自分で足す関数の中身は cpp に置く。一般規則の「ヘッダに実装を書くと 0 点」は、課題が明示したこの形を除いて守る。

### 7.2 抽象クラスとインターフェースの違い（この課題の中）

| | `AMateria` | `ICharacter` / `IMateriaSource` |
|--|------------|----------------------------------|
| データ | `type` を持つ | 持たない |
| 関数 | `clone()` は純粋仮想。`use()` は virtual だが本体を持てる。コンストラクタがある | 操作はすべて純粋仮想 |
| 呼び方 | 抽象クラス | インターフェース |
| 役割 | 「マテリアとは何か」の共通実装と、複製という穴 | 「キャラクターが何を約束するか」だけの契約 |

`AMateria` をインターフェースと呼ばないのは、型名の文字列とコンストラクタと `use` のデフォルトを持っているから。共通の実装を少し持つ抽象クラスと、契約だけのインターフェースは、Scott Meyers の言い方では **実装の継承** と **インターフェースの継承** に分かれる。

`Character` は `ICharacter` を public 継承する。is-a が成り立つ。`Character` は `ICharacter` として扱える。`ICharacter* me = new Character("me");` が課題の main そのもの。

`Ice` は `AMateria` を継承し、`clone()` を実装して具象クラスになる。

### 7.3 なぜキャラクターは Ice を知らないのか

`Character` のインベントリの型は `AMateria*`。`Ice*` でも `Cure*` でもない。

```text
  Character                         AMateria*
  ┌─────────────┐                   ┌──────────────┐
  │ slot[0] ────┼──────────────────►│ Ice          │ use() は氷の文
  │ slot[1] ────┼──────────────────►│ Cure         │ use() は治療の文
  │ slot[2]     │ null              └──────────────┘
  │ slot[3]     │ null
  └─────────────┘
```

`Character::use` がやることは「そのスロットの `use` を呼ぶ」だけ。氷の文章は `Ice` が知っている。新しい `Fire` を足すとき、変更するのは `Fire` クラスの追加と、それを生成する場所だけ。`Character.cpp` は開かない。

この性質を設計の言葉では **オープン・クローズド**（拡張に開き、修正に閉じる）と呼ぶことがある。評価中の修正課題「炎マテリアを足して」は、この性質をその場で確認するためのもの。

依存の向きも同じ。上位（キャラクター）が下位（氷という具象）に依存しない。両方が抽象（`AMateria` と `ICharacter`）に依存する。

これは [4.10](#410-c-で書くと同じ構造になる) の `file_operations` と同じ切り方である。VFS が ext4 の `read` を知らないように、`Character` は `Ice::use` の文面を知らない。知っているのは「スロットの `use` を呼ぶ」という欄だけである。

### 7.4 前方宣言と循環インクルード

`AMateria::use` は `ICharacter&` を受け取る。`ICharacter::equip` は `AMateria*` を受け取る。ヘッダ同士が `#include` し合うと循環する。

ポインタと参照の **宣言** には、相手の完全な定義は要らない。前方宣言で足りる。メンバ関数の **中で** `getName()` のように相手の中身を使う cpp では、完全なヘッダを include する。

```text
  AMateria.hpp
      class ICharacter;          ← 前方宣言だけ
      void use(ICharacter& target);

  ICharacter.hpp
      class AMateria;            ← 前方宣言だけ
      void equip(AMateria* m);

  Ice.cpp
      #include "Ice.hpp"
      #include "ICharacter.hpp"  ← getName() を呼ぶので完全な定義が要る
      #include <iostream>

  Character.hpp
      #include "ICharacter.hpp"
      class AMateria;            ← スロットはポインタなので前方宣言で足りる
      AMateria* inventory[4];

  Character.cpp
      #include "Character.hpp"
      #include "AMateria.hpp"    ← delete と clone() と use() に完全な定義が要る
```

`AMateria.hpp` から `Character.hpp` を include しない。具象を抽象が知った瞬間、依存が逆になる。

各ヘッダは、それ単体で include できること。使う型の定義か前方宣言を、そのヘッダ自身が持つ。インクルードガードは必須。

```cpp
#ifndef AMATERIA_HPP
#define AMATERIA_HPP
// ...
#endif
```

`#pragma once` は C++98 の標準ではない。`#ifndef` を使う。

---

## 8. 所有権・浅いコピー・深いコピー

このモジュールのバグの大半は、virtual の理解ではなく **「この new を誰が delete するか」が途中で消える**こと。

### 8.1 所有権は 1 つの new につき 1 人

```text
  new した瞬間に、delete する人を 1 人決める。
  その人が死ぬとき、まだ持っているものを delete する。
  人から人へ渡すときは、渡した側はもう delete しない。
```

`std::string` や `Brain` の中の `std::string ideas[100]` は、文字列自身が中身のメモリを管理する。`Brain` のコピーは、コンパイラが生成するコピーでも、文字列については深いコピーになる。**自分で `new` したポインタだけが、自分で書く対象。**

課題が `Brain` を値メンバではなく `Brain*` にさせているのは、この自動機構が無い状況を練習させるため。値メンバなら `Dog` のコピーは何もしなくても `Brain` ごと複製される。

### 8.2 浅いコピー

```cpp
Dog::Dog(Dog const &other) : Animal(other), brain(other.brain) {}
```

```text
  original.brain ──┐
                   ├──► 同じ 1 個の Brain
  copy.brain ──────┘

  ~copy が delete brain
  ~original がもう一度 delete brain   → 二重解放。未定義動作
```

アイデアを書き換えると両方から見える。テストが「コピーのアイデアを変えたら元も変わった」なら、浅いコピー。

### 8.3 深いコピー

```cpp
Dog::Dog(Dog const &other)
    : Animal(other), brain(new Brain(*other.brain)) {}
```

```text
  original.brain ──► Brain A   ideas[0] = "chase"
  copy.brain     ──► Brain B   ideas[0] = "chase"   ← 別のオブジェクト、中身は同じ
```

その後 `copy` のアイデアを `"sleep"` に変えても、`original` は `"chase"` のまま。アドレスも違う。

`Animal(other)` を忘れない。忘れると基底のデフォルトコンストラクタが走り、`type` が `"Dog"` にならない。Module 03 と同じ穴が、資源のコピーとセットで再登場する。

代入はこう書ける。`Brain` は構築時に必ず 1 個ある前提。

```cpp
Dog &Dog::operator=(Dog const &other) {
    if (this != &other) {
        Animal::operator=(other);
        *this->brain = *other.brain;     // Brain の代入が 100 個の string をコピーする
    }
    return *this;
}
```

既にある `Brain` に中身をコピーしているので、`delete` して `new` し直さなくてよい。自己代入で自分の `Brain` を先に消してから自分をコピーする、という壊れ方も起きにくい。

`delete` してから `new` する書き方もある。

```cpp
Brain *fresh = new Brain(*other.brain);  // 先に作る
delete this->brain;
this->brain = fresh;
```

先に消してから `new` すると、`new` が失敗したとき（`std::bad_alloc`。本格的には Module 05）にポインタが宙ぶらりんになる。この課題ではそこまで求められない。`*brain = *other.brain` の方が短い。

**自己代入チェックを消して、先に `delete this->brain` してから `other.brain` をコピーすると、`d = d` で解放済みメモリを読む。** チェックは残す。

`std::swap` を使った Copy-and-Swap は、C++98 では `<algorithm>` にあり、Module 08 まで STL アルゴリズムは禁止。ここでは使わない。Module 02 のノートにあるとおり。

### 8.4 Rule of Three がここで本番になる

デストラクタで `delete` するクラスは、コピー構築とコピー代入も自分で書く。どれか 1 つだけだと、残りが「ポインタをビットコピーする」デフォルトになり、浅いコピーになる。

このモジュールでそれが該当するのは `Dog`、`Cat`、`Character`、`MateriaSource`。

`Brain` は `std::string` の配列を値で持つので、デフォルトのコピーでも深い。OCF として 4 つは明示的に書き、コピーでは 100 要素をループで移す。`<algorithm>` の `std::copy` は使わない。

`Animal` は `std::string` しか持たないので資源管理の必然性は薄い。モジュール規則の OCF と、コンストラクタ／デストラクタのメッセージのために 4 つ書く。

### 8.5 代入演算子は virtual にしなくてよい

```cpp
Animal &a = dog;
Animal &b = cat;
a = b;    // Animal::operator= だけが走る。Dog の Brain はコピーされない
```

代入は、課題のテストでは `Dog` や `Character` という具体型に対して行う。基底参照への代入で派生部分までコピーしたければ、ex03 の `clone()` のような仮想的な複製が要る。`operator=` を virtual にしても戻り値の型の問題が残り、この課題の範囲外。

`clone()` は「基底ポインタしか無いときに、動的な型のコピーを作る」ための virtual 関数。ex01 は具体型が分かっているのでコピーコンストラクタで足りる。ex03 は `AMateria*` しか無いので `clone()` が必要になる。この対比が、2 つの演習が同じ「深いコピー」という言葉で繋がっている理由。

### 8.6 配列の delete と delete[]

```cpp
Animal *zoo[4];
zoo[0] = new Dog();
zoo[1] = new Dog();
zoo[2] = new Cat();
zoo[3] = new Cat();

for (int i = 0; i < 4; ++i)
    delete zoo[i];          // 各要素は new したので delete
```

`zoo` 自体はスタック上の配列。`delete[] zoo` は書かない。`new[]` していない。

`Animal *zoo = new Animal*[4];` とヒープにポインタ配列を置いた場合だけ、各 `delete zoo[i]` のあとに `delete[] zoo` が要る。要素の `delete` と、入れ物の `delete[]` は別。

### 8.7 これは、あとで型が持つ所有権の手書き版である

`std::unique_ptr` はこの課程では使えない。C++11 であり、ここまでのモジュールは STL の所有権ラッパを課題に出していない。

やっていることの対応だけを残す。

| 今、手で書いていること | 現代の C++ が型で書くこと |
|------------------------|---------------------------|
| デストラクタで `delete` する | `unique_ptr` のデストラクタが `delete` する |
| コピーを自分で深く書く。さもないと 2 人が同じ塊を `free` する | `unique_ptr` はコピーできない。移すなら所有者が 1 人のまま移る |
| 基底ポインタで消すなら virtual デストラクタ | `unique_ptr<Animal>` も、消す型のデストラクタが virtual でなければ同じ未定義動作 |
| `clone()` で動的な型の複製を作る | コピーを禁止した型が複製を欲しがるとき、仮想の `clone` を自分で書くのは今と同じ |

Rule of Three は、この所有権を言語機能がまだ型に入れてくれない時代の規則である。ex01 と ex03 で手を動かすのは、後から `unique_ptr` を見たときに「コンパイラとライブラリが、あの `delete` の担当を型へ移した」と分かるためである。

---

## 9. ex00 — ポリモーフィズム

ディレクトリ: `ex00/`
提出: `Makefile`, `main.cpp`, `*.cpp`, `*.{h,hpp}`

### 9.1 クラスの関係

```text
  Animal
    ├── Dog
    └── Cat

  WrongAnimal
    └── WrongCat
```

`WrongDog` は課題が要求していない。足してもよいが、説明の対比は `WrongCat` だけで足りる。

### 9.2 Animal

| 項目 | 内容 |
|------|------|
| `protected: std::string type;` | 課題が protected と指定している。private にしない |
| 初期値 | 空でも、`"Animal"` でもよい。テストで見分けやすいので `"Animal"` を勧める |
| `std::string const & getType() const` | 参照を返すとコピーが要らない。const オブジェクトから呼べる |
| `virtual void makeSound() const` | メッセージは派生と違う文にする |
| `virtual ~Animal()` | メッセージを出す。ex01 の `delete` に備えて virtual |

`protected` はカプセル化を弱める（派生なら誰でも `type` を後から書き換えられる）。課題が属性の置き場所を指定しているので従う。より閉じた設計は「`Animal` のコンストラクタが文字列を受け取り、外からは `getType()` だけ」だが、属性自体は protected のまま残る。評価では「課題指定だから protected。書き込みを構築時に寄せている」と言えればよい。

コンストラクタとデストラクタのメッセージはクラスごとに変える。全クラスで同じ文にしない、と課題にある。

### 9.3 Dog と Cat

デフォルトコンストラクタで `type` を `"Dog"` / `"Cat"` にする。

```cpp
Dog::Dog() : Animal() {
    this->type = "Dog";
    std::cout << "Dog constructor called" << std::endl;
}

void Dog::makeSound() const {
    std::cout << "Woof" << std::endl;
}

void Cat::makeSound() const {
    std::cout << "Meow" << std::endl;
}
```

猫の文を犬と同じにしない。課題は「猫は吠えない」と書いている。文面の指定は ex00 には無い。区別が付けばよい。`std::cout`、末尾は改行。`std::cerr` や `printf` は使わない。

コピーコンストラクタは `: Animal(other)` で `type` を引き継ぐ。本体で `"Dog"` を代入し直してもよい。メッセージはデフォルト構築と別の文にする（「コピーされた」と分かる文）。

### 9.4 課題の main が示すこと

```cpp
const Animal* meta = new Animal();
const Animal* j = new Dog();
const Animal* i = new Cat();
std::cout << j->getType() << " " << std::endl;
std::cout << i->getType() << " " << std::endl;
i->makeSound();
j->makeSound();
meta->makeSound();
```

期待の骨格:

```text
Dog
Cat
（猫の鳴き声）
（犬の鳴き声）
（Animal の鳴き声）
```

`getType()` の前後にスペースと改行があるのは、課題の `<< " " << std::endl` のまま。

この main には `delete` が無い。提示コードをそのまま提出用にすると 3 匹分リークする。提出用 main では `delete meta; delete j; delete i;` を足し、デストラクタが派生→基底の順で出ることを自分の目で見る。評価者が提示 main を貼り直した場合、リークは提示 main 側にある、と説明できる。

### 9.5 WrongAnimal / WrongCat

構造は `Animal` / `Cat` と同じに見える。違いは **`makeSound` が virtual ではない**こと。

```cpp
const WrongAnimal *wrong = new WrongCat();
wrong->makeSound();          // WrongAnimal の鳴き声

WrongCat direct;
direct.makeSound();          // WrongCat 自身の鳴き声（静的な型が WrongCat だから）

delete wrong;                // ~WrongAnimal を virtual にしてあれば ~WrongCat も走る
```

`getType()` は virtual でなくても `"WrongCat"` になりうる。`WrongCat` のコンストラクタが `type` を書いたあとに、基底の `getType()` がそのメンバを読むから。評価で「Wrong なのに getType は WrongCat なんですね」と言われたとき、第3章の「データと関数本体は別」で答える。

非 virtual の `makeSound` を派生が再定義するのは、Module 03 の名前隠蔽。オーバーライドではない。

### 9.6 ex00 のファイル

| ファイル | 役割 |
|----------|------|
| `Animal.hpp` / `Animal.cpp` | 基底 |
| `Dog.hpp` / `Dog.cpp` | 派生 |
| `Cat.hpp` / `Cat.cpp` | 派生 |
| `WrongAnimal.hpp` / `WrongAnimal.cpp` | 非 virtual 版の基底 |
| `WrongCat.hpp` / `WrongCat.cpp` | 非 virtual 版の派生 |
| `main.cpp` | 課題の例 + delete + Wrong の対比 + コピー |
| `Makefile` | `c++ -Wall -Wextra -Werror -std=c++98` |

`Dog.hpp` は `Animal.hpp` を include する。継承には基底の完全な定義が要る。前方宣言では継承できない（Module 03 と同じ）。

---

## 10. ex01 — Brain と深いコピー

ディレクトリ: `ex01/`
提出: ex00 のファイル一式に、`Brain` と深いコピーを足したもの。

ex00 フォルダは触らず、`ex01` にコピーしてから改造する。

### 10.1 Brain

```cpp
class Brain {
    private:
        std::string ideas[100];
    public:
        Brain();
        Brain(Brain const &other);
        ~Brain();
        Brain &operator=(Brain const &other);

        void setIdea(int index, std::string const &idea);
        std::string const &getIdea(int index) const;
};
```

配列の名前は課題の `ideas` を使う。中身は private にし、テスト用に setter / getter を足してよい。課題は必須ファイルを出していれば追加の関数を許している。

`std::string` のデフォルト構築で 100 要素は空文字になる。要素を `new` しない。

インデックスは 0 以上 100 未満だけを受け付ける。範囲外は何もしない、または空文字を返す、と自分で決めて main で守る。未定義の添字アクセスをテストに入れない。

コンストラクタとデストラクタは、他クラスと違うメッセージを出す。

### 10.2 Dog と Cat が Brain を持つ

```cpp
class Dog : public Animal {
    private:
        Brain *brain;
    public:
        Dog();
        Dog(Dog const &other);
        ~Dog();
        Dog &operator=(Dog const &other);
        void makeSound() const;
        Brain *getBrain() const;
        void setIdea(int index, std::string const &idea);
};
```

`Cat` も同じ形。`Animal` は `Brain` を持たない。持つのは犬と猫だけ、と課題が指定している。

```cpp
Dog::Dog() : Animal(), brain(new Brain()) {
    this->type = "Dog";
    std::cout << "Dog constructor called" << std::endl;
}

Dog::Dog(Dog const &other)
    : Animal(other), brain(new Brain(*other.brain)) {
    std::cout << "Dog copy constructor called" << std::endl;
}

Dog::~Dog() {
    std::cout << "Dog destructor called" << std::endl;
    delete this->brain;
}

Dog &Dog::operator=(Dog const &other) {
    if (this != &other) {
        Animal::operator=(other);
        *this->brain = *other.brain;
    }
    std::cout << "Dog assignment called" << std::endl;
    return *this;
}
```

構築メッセージの順序（デフォルト構築）:

```text
Animal constructor
Brain constructor
Dog constructor
```

`brain(new Brain())` は `Dog` の本体より前。`Animal()` はそれより前。初期化子リストに `brain` を `Animal` より先に書いても、実行は基底が先。書いた順と宣言順が違うと `-Wreorder` が `-Werror` でエラーになる。基底を先に書く。

破棄:

```text
Dog destructor
Brain destructor
Animal destructor
```

`getBrain()` が非 const の `Brain*` を返すのはテストからアイデアを書くため。公開したくなければ `setIdea` だけを `Dog` に置く。深いコピーの証明には、中身を読んで書ける経路が 1 つ要る。

### 10.3 半分ずつの配列

課題は「`Animal` の配列を作り、半分を `Dog`、半分を `Cat` で埋め、最後に `Animal` として `delete` する」と書いている。奇数だと半分が割れないので、偶数にする。

```cpp
const int count = 4;
Animal *animals[4];

for (int i = 0; i < count; ++i) {
    if (i < count / 2)
        animals[i] = new Dog();
    else
        animals[i] = new Cat();
}

for (int i = 0; i < count; ++i)
    delete animals[i];
```

`Animal animals[4];` は、ex02 以降は抽象クラスなのでコンパイルできず、具象のままでもスライスする。**ポインタの配列**。

各 `delete` で `~Dog` または `~Cat`、その中で `~Brain`、そのあと `~Animal` が出る。`~Animal` しか出なければ、デストラクタが virtual になっていない。

課題の小さい main も残す。

```cpp
const Animal *j = new Dog();
const Animal *i = new Cat();
delete j;
delete i;
```

ここだけで Brain のリークは検出できる。配列テストと深いコピーテストは「提示以上」として足す。

### 10.4 深いコピーのテスト

クラッシュさせないテストにする。浅いコピーのまま「2 回デストラクタが走るか」を見ると、二重 `delete` の未定義動作で、環境によって落ちたり落ちなかったりする。落ちなかったことを正しさと混同しない。

```cpp
Dog original;
original.setIdea(0, "chase tail");

Dog copy(original);
copy.setIdea(0, "sleep");

// アドレスが違う
// original の idea 0 は "chase tail" のまま
// copy の idea 0 は "sleep"

Dog assigned;
assigned = original;
assigned.setIdea(0, "eat");
// original はまだ "chase tail"
```

コピー構築と代入の両方。課題は「コピー」とだけ書いているので、OCF の両方を見せる。

`Cat` でも同じテストを 1 つ。片方だけ直して片方を浅いコピーのまま、がよくある。

### 10.5 スタックの犬を delete しない

```cpp
Dog stackDog;
Animal *p = &stackDog;
delete p;     // 未定義動作。new していない
```

`new` と `delete`、自動変数とスコープ終了、は別の寿命。テストでは混ぜない。

---

## 11. ex02 — 抽象クラス

ディレクトリ: `ex02/`
提出: ex01 のファイル一式をコピーし、`Animal` を抽象クラスにしたもの。

変更の中心は 1 行。

```cpp
virtual void makeSound() const = 0;
```

`Animal::makeSound` の cpp 定義は削除する。デストラクタは定義を残す。メッセージも残す。

消すもの、残すもの。

| コード | ex02 |
|--------|------|
| `new Animal()` / `Animal a;` | 消す。コンパイルエラーが正しい状態 |
| `Animal *p = new Dog();` | 残す |
| 半分ずつの配列と `delete` | 残す |
| 深いコピー | 残す |
| `WrongAnimal` | 具象のままでよい。課題が抽象にしろと言っているのは `Animal` |
| `Dog::makeSound` の実装 | 残す。無いと `Dog` も抽象になる |

`makeSound` を純粋仮想にしたあと、どこかに `Animal` の値や `new Animal()` が残っていると、その翻訳単位がコンパイルエラーになる。エラーは失敗ではなく、ex02 の目的が効いている印。残っている呼び出しを消す。

動作の回帰: 犬と猫の鳴き声、`getType()`、`delete` の順序、深いコピー。ex01 と同じ結果。

---

## 12. ex03 — インターフェース

ディレクトリ: `ex03/`
提出: `Makefile`, `main.cpp`, `*.cpp`, `*.{h,hpp}`

ex00〜ex02 のファイルは **提出物に含まれない**。新しいプログラム。`Animal` は出てこない。

課題が提示した main の出力は、次の 2 行だけ。

```text
* shoots an ice bolt at bob *
* heals bob's wounds *
```

`cat -e` の行末 `$` は「改行があった」という表示。プログラムが `$` を出力するわけではない。

**ex03 のクラスのコンストラクタとデストラクタではメッセージを出さない。** ex00 / ex01 の「必ずメッセージを出せ」は ex03 には書かれていない。出すと、提示 main をそのままコンパイルしたときの出力が 2 行ではなくなる。追加テスト用の表示は `main.cpp` に書く。

### 12.1 クラス一覧

```text
  IMateriaSource ◄──── MateriaSource
                              │ 最大 4 個を所有（テンプレート）
                              ▼
                          AMateria
                           ├── Ice     type は "ice"
                           └── Cure    type は "cure"

  ICharacter ◄──── Character("名前")
                        │ 最大 4 個を所有（装備）
                        ▼
                     AMateria
```

| クラス | 具象か | OCF |
|--------|--------|-----|
| `AMateria` | 抽象（`clone` が純粋仮想） | 書く。自分ではインスタンス化できないが、基底部分として必要 |
| `Ice`, `Cure` | 具象 | 書く |
| `ICharacter`, `IMateriaSource` | インターフェース | 課題が示した形のまま。コピーは定義しない。インスタンス化しない |
| `Character`, `MateriaSource` | 具象 | 書く。ポインタを所有するのでコピーは深い |

### 12.2 AMateria

課題の骨格に、OCF と virtual デストラクタを足した形。

```cpp
class AMateria {
    protected:
        std::string type;
    public:
        AMateria(std::string const &type);
        AMateria(AMateria const &other);
        virtual ~AMateria();
        AMateria &operator=(AMateria const &other);

        std::string const &getType() const;
        virtual AMateria *clone() const = 0;
        virtual void use(ICharacter &target);
};
```

`type` の初期化で、引数名とメンバ名が同じになる。

```cpp
AMateria::AMateria(std::string const &type) : type(type) {}
```

初期化子リストの `type(type)` は「メンバ `type` を、引数 `type` で初期化する」。これは正しい。

本体で書くときは影に注意する。

```cpp
AMateria::AMateria(std::string const &type) {
    type = type;          // 引数を引数に代入している。メンバは空のまま
    this->type = type;    // メンバへ入る
}
```

`-Wshadow` は `-Wall` に含まれない。このバグはコンパイルが通り、`createMateria("ice")` が永遠に 0 を返す、という形で出る。`Ice` の `use` の文章はタイプ文字列を見ないので、装備して使うだけだと気づかない。

**代入では type をコピーしない。** 課題がそう書いている。`Ice` の type が代入で `"cure"` になる、という状態を作らないため。コピーコンストラクタは type をコピーしてよい。構築中の複製は、同じタイプのマテリアを作る正当な経路。禁止されているのは代入の方。

```cpp
AMateria::AMateria(AMateria const &other) : type(other.type) {}

AMateria &AMateria::operator=(AMateria const &other) {
    (void)other;          // type は変えない。-Wextra の未使用引数を避けるなら引数名を省略してもよい
    return *this;
}

void AMateria::use(ICharacter &target) {
    (void)target;
}
```

`use` は純粋仮想ではない。空の本体でよい。`Ice` と `Cure` がオーバーライドする。`Character` は `AMateria*` 経由で呼ぶので、virtual であることが効く。

デストラクタは virtual。`Character` は `AMateria*` として `delete` する。`Ice` が今は追加資源を持たなくても、非 virtual の `delete` は未定義動作。メッセージは出さない（上の出力の話）。

引数名を省略すると未使用警告を避けられる。`-Wextra` は未使用引数を警告し、`-Werror` でエラーになる。

```cpp
void AMateria::use(ICharacter &) {}
```

### 12.3 Ice と Cure

タイプは **小文字**。クラス名 `Ice` と文字列 `"ice"` は別。

```cpp
Ice::Ice() : AMateria("ice") {}

Ice::Ice(Ice const &other) : AMateria(other) {}

Ice &Ice::operator=(Ice const &other) {
    if (this != &other)
        AMateria::operator=(other);    // type は変わらない
    return *this;
}

AMateria *Ice::clone() const {
    return new Ice(*this);
}

void Ice::use(ICharacter &target) {
    std::cout << "* shoots an ice bolt at " << target.getName() << " *"
              << std::endl;
}
```

`Cure` は `"cure"` と、次の文。

```text
* heals <名前>'s wounds *
```

山括弧は出さない。`bob` なら `* heals bob's wounds *`。アポストロフィの位置は課題の文のとおり `bob's`。

`clone` の戻り値型は `AMateria*` でよい。C++98 には共変戻り値型があり、派生では `Ice*` を返す宣言も合法（基底が `AMateria*` を返し、派生がより派生側のポインタを返す）。課題の宣言に合わせて `AMateria*` で十分。`new Ice()` でも「同じタイプの新しいインスタンス」は満たせる。`new Ice(*this)` にしておくと、将来フィールドが増えてもコピーされる。

出力の空白: アスタリスク、空白、文、空白、アスタリスク、改行。`*shoots` や `bob*` のように空白を落とすと期待とずれる。

### 12.4 ICharacter と Character

課題の `ICharacter` はそのまま実装する。関数の追加で純粋仮想を消したり、引数を変えたりしない。

`Character` の契約。

| 操作 | 契約 |
|------|------|
| 構築 `Character(std::string const &name)` | 名前を保持。スロット 4 つはすべて空（0） |
| `equip(m)` | `m` が 0 なら何もしない。スロット 0 から見て最初の空きに **そのポインタをそのまま**入れる。空きが無ければ何もしない。何もしないとき `delete` しない |
| `unequip(idx)` | 範囲外、または空なら何もしない。範囲内ならスロットを 0 にする。**delete しない** |
| `use(idx, target)` | 範囲外または空なら何もしない。そうでなければ `inventory[idx]->use(target)` |
| 破棄 | スロットに残っているマテリアを `delete` |
| コピー構築 | 名前をコピー。相手の各マテリアを `clone()` して自分のスロットへ。相手とポインタを共有しない |
| コピー代入 | 自己代入なら何もしない。名前をコピー。**今持っているマテリアを先に delete** し、そのあと相手を `clone()` して入れる |

スロット数はマジックナンバーのまま散らさず、クラス内の `static const int` か enum にする。C++98 では整数の `static const` はクラス内で初期化できる。

```cpp
static const int kSlotCount = 4;
```

配列メンバは初期化子リストで 0 埋めしにくい。コンストラクタ本体でループする。**未初期化のポインタをデストラクタが `delete` すると未定義動作。** 全コンストラクタで 4 つを 0 にする。

```cpp
Character::Character(std::string const &name) : name(name) {
    for (int i = 0; i < kSlotCount; ++i)
        this->inventory[i] = 0;
}
```

`equip` の「最初の空き」は、穴を飛ばさない。

```text
  [ice, cure, 0, 0]
  unequip(0)
  [0, cure, 0, 0]
  equip(新しい氷)
  [新しい氷, cure, 0, 0]     ← スロット 2 ではない
```

範囲は `int`。負数が来る。`idx < 0 || idx >= 4` を弾く。`unsigned` にすると負数が巨大な正になり、範囲チェックの意味が変わる。

`use` という名前が 2 つある。混同しやすいので、呼び出しの形で覚える。

| 誰の use か | 引数の意味 |
|-------------|------------|
| `Character::use(int, ICharacter&)` | 何番のスロットを、どのキャラクターに対して使うか |
| `AMateria::use(ICharacter&)` | このマテリアを、そのキャラクターに使うと何が起きるか |

`Character::use` の本体は、スロットのマテリアの `use` に `target` を渡すだけ。文章を `Character` に書かない。

```cpp
void Character::use(int idx, ICharacter &target) {
    if (idx < 0 || idx >= kSlotCount || this->inventory[idx] == 0)
        return;
    this->inventory[idx]->use(target);
}
```

同じポインタを 2 つのスロットに入れると、破棄時に二重 `delete` になる。課題は明示していない。`equip` の前に「すでにこのアドレスを持っているなら何もしない」と決めておくと、その事故が消える。評価で聞かれたら「二重解放を避けるための自分の契約」と言える。

### 12.5 床に置いたマテリア

`unequip` は手放すだけで、消さない。手放したあとのアドレスを誰も持っていないと、リークが確定する。課題は「呼ぶ前にアドレスを保存せよ。リークは避けること」と書いている。

提出用の基本方針は、**床クラスを作らず、テストがポインタを保持する**。

```cpp
AMateria *tmp = src->createMateria("ice");
me->equip(tmp);
me->unequip(0);
delete tmp;                 // unequip は消していない。まだ有効
me->use(0, *bob);           // スロットは空。何も出ない
```

`equip` が同じポインタを格納するから、この `delete tmp` が正しい相手を消せる。

床を自動回収したくなった場合の条件:

- `std::vector` は使えない
- `unequip` の中で `delete` しない（課題が禁止）
- 回収役が最後に `delete` する
- 固定長配列か、自分で書いた連結リストなら STL 禁止に抵触しない

課題の文に一番素直なのは、呼び出し側が保存して消す方。追加の床クラスは、その契約を崩さない範囲の任意機能。

### 12.6 コピーで「先に消してから入れる」

課題: コピーのとき、キャラクターが持っているマテリアは、新しいものが追加される前に削除される。破棄時にも削除される。

コピー構築では、消すべき旧マテリアがまだ無い。スロットを 0 にしたあと `clone()` する。未初期化ポインタを `delete` しない。

```cpp
Character::Character(Character const &other) : name(other.name) {
    for (int i = 0; i < kSlotCount; ++i)
        this->inventory[i] = 0;
    for (int i = 0; i < kSlotCount; ++i) {
        if (other.inventory[i] != 0)
            this->inventory[i] = other.inventory[i]->clone();
    }
}
```

代入では、先に自分のものを消す。自己代入を先に返す。自分を消してから自分を `clone()` すると、解放済みを複製することになる。

```cpp
Character &Character::operator=(Character const &other) {
    if (this != &other) {
        this->name = other.name;
        for (int i = 0; i < kSlotCount; ++i) {
            delete this->inventory[i];
            this->inventory[i] = 0;
        }
        for (int i = 0; i < kSlotCount; ++i) {
            if (other.inventory[i] != 0)
                this->inventory[i] = other.inventory[i]->clone();
        }
    }
    return *this;
}

Character::~Character() {
    for (int i = 0; i < kSlotCount; ++i)
        delete this->inventory[i];
}
```

`delete` は 0 に対しては何もしない、と規格が定めている。空スロットを `delete` してよい。その前に 0 初期化してあることが条件。

名前もコピーする。課題はマテリアの深さだけを明示しているが、コピーが同じキャラクターとして振る舞うなら名前は値としてコピーされる。OCF の自然な意味がそれ。

デストラクタが virtual な経路: `ICharacter* me = new Character("me"); delete me;`
`~ICharacter` が virtual なので `~Character` が走り、その中でマテリアが消える。インターフェースに virtual デストラクタがあるのは、ex01 の `~Animal` と同じ理由。

### 12.7 MateriaSource

```cpp
class IMateriaSource {
    public:
        virtual ~IMateriaSource() {}
        virtual void learnMateria(AMateria*) = 0;
        virtual AMateria* createMateria(std::string const &type) = 0;
};
```

`learnMateria` の文は「渡されたマテリアをコピーし、後でクローンできるよう記憶する」。提示 main は `learnMateria(new Ice())` と書き、そのポインタを変数に残さない。最後の `delete src` だけで終わらせている。

この 2 つを同時に満たす契約は次。

> `learnMateria` は渡されたポインタの所有権を受け取る。空いている枠にそのポインタを保存する。`createMateria` が、保存してあるものを `clone()` して新しい個体を返す。テンプレートの `delete` は `MateriaSource` のデストラクタが行う。

`learnMateria` の中でさらに `clone()` し、引数を放置すると、提示 main の `new Ice()` と `new Cure()` がリークする。「コピー」という言葉に寄せて中で `clone()` し、引数をその場で `delete` する読みもある。その契約では、呼び出し側が渡したポインタは関数から戻った時点で無効になる。提示 main はポインタを使わないので動くが、呼び出し側にとっては驚きが大きい。**保存するのは渡されたポインタそのもの**とし、複製の責務は `createMateria` に置く。評価では「提示 main がアドレスを保持しないので、所有権は MateriaSource に移ると読んだ」と言えればよい。

枠が満杯のとき、課題は `equip` については「何も起きない」と書いている。`learnMateria` については書いていない。提示 main の呼び方はアドレスを捨てるので、次まで決めておくとリークしない。

> 所有権は呼んだ瞬間に常に移る。枠が無く保存できないとき、`MateriaSource` が `delete` する。引数が 0 なら何もしない。

`equip` はアドレスが呼び出し側の変数に残るので、満杯でも `delete` しない。2 つの関数で契約が違う。表にして覚える。

| 関数 | 成功したとき誰が delete するか | 失敗したとき |
|------|--------------------------------|--------------|
| `equip` | `Character` が破棄時に delete | 呼び出し側がまだ所有者。関数は delete しない |
| `unequip` | 所有権が呼び出し側に戻る。関数は delete しない | 何もしない |
| `learnMateria` | `MateriaSource` が破棄時に delete | 受け取ったものは MateriaSource が delete する、と決める |
| `createMateria` | 戻り値の所有権は呼び出し側。`equip` するか、自分で `delete` する | 0 を返す。delete するものは無い |

未知のタイプは 0 を返す。`NULL` を使うなら `<cstddef>` が要る。C++98 では `0` でよい。`nullptr` は C++11 なので使わない。

同じタイプを複数回 `learn` してよい。課題は「一意でなくてよい」と書いている。`createMateria` は先頭から見て、タイプが一致した最初のテンプレートを `clone()` する、と決める。

```cpp
AMateria *MateriaSource::createMateria(std::string const &type) {
    for (int i = 0; i < kSlotCount; ++i) {
        if (this->templates[i] != 0 && this->templates[i]->getType() == type)
            return this->templates[i]->clone();
    }
    return 0;
}
```

比較は大文字小文字を区別する。`"Ice"` は `"ice"` と一致しない。

`MateriaSource` のコピーも深くする。課題文が「深い」と明記しているのは `Character` の方。`MateriaSource` もポインタを所有するので、Rule of Three により同じことが必要。浅いコピーのまま 2 人を破棄すると二重 `delete`。説明は「明記は Character。MateriaSource は所有しているから同じ」。

テンプレート枠も構築時に 0 で埋める。破棄時に `delete` する。

### 12.8 提示 main を 1 行ずつ追う

```cpp
IMateriaSource* src = new MateriaSource();
src->learnMateria(new Ice());          // src が Ice テンプレートを所有
src->learnMateria(new Cure());         // src が Cure テンプレートを所有
ICharacter* me = new Character("me");
AMateria* tmp;
tmp = src->createMateria("ice");       // テンプレートとは別の新しい Ice。所有はまだ tmp
me->equip(tmp);                        // スロット 0。所有は me へ。tmp は同じアドレスを覚えているだけ
tmp = src->createMateria("cure");      // 新しい Cure
me->equip(tmp);                        // スロット 1
ICharacter* bob = new Character("bob");
me->use(0, *bob);                      // スロット 0 は Ice → ice bolt at bob
me->use(1, *bob);                      // スロット 1 は Cure → heals bob
delete bob;                            // 空のインベントリを破棄
delete me;                             // Ice と Cure のクローンを破棄してから Character
delete src;                            // テンプレートの Ice と Cure を破棄
```

`new` は 7 回。それぞれに `delete` が 1 つある。

| new | delete する人 |
|-----|----------------|
| `new MateriaSource` | `delete src` |
| `new Ice`（learn） | `~MateriaSource` |
| `new Cure`（learn） | `~MateriaSource` |
| `new Character("me")` | `delete me` |
| `createMateria("ice")` の `new Ice` | `~Character` of me |
| `createMateria("cure")` の `new Cure` | `~Character` of me |
| `new Character("bob")` | `delete bob` |

7 個の `new` に 7 個の `delete`。クローンとテンプレートは別オブジェクト。`me` を消しても `src` の見本は残っている。そのあと `src` が見本を消す。

`use` の `<name>` は `target.getName()`。`bob` の名前。使う側 `me` の名前ではない。

### 12.9 循環しないファイル構成

| ファイル | include するもの |
|----------|------------------|
| `AMateria.hpp` | `<string>`、`ICharacter` は前方宣言 |
| `AMateria.cpp` | `AMateria.hpp`。`use` が相手を触らないなら `ICharacter.hpp` は必須ではない |
| `ICharacter.hpp` | `<string>`、`AMateria` は前方宣言 |
| `IMateriaSource.hpp` | `<string>`、`AMateria` は前方宣言 |
| `Ice.hpp` | `AMateria.hpp` |
| `Ice.cpp` | `Ice.hpp`, `ICharacter.hpp`, `<iostream>` |
| `Cure.hpp` / `Cure.cpp` | Ice と同じ形 |
| `Character.hpp` | `ICharacter.hpp`、`AMateria` は前方宣言 |
| `Character.cpp` | `Character.hpp`, `AMateria.hpp` |
| `MateriaSource.hpp` | `IMateriaSource.hpp`、`AMateria` は前方宣言 |
| `MateriaSource.cpp` | `MateriaSource.hpp`, `AMateria.hpp` |
| `main.cpp` | 具象クラスのヘッダ一式 |

`clone()` の定義をヘッダのクラス本体に書かない。

### 12.10 評価中に頼まれやすい修正

`Fire` を足す。既存の `Character.cpp` は変更しない。

1. `Fire.hpp` / `Fire.cpp`。`AMateria("fire")`。`clone` は `new Fire(*this)`。`use` は自分で決めた 1 行。
2. main で `src->learnMateria(new Fire());` と `createMateria("fire")` と `equip` と `use`。
3. Makefile のソース一覧に `Fire.cpp` を足す。

これが 10 分でできる構造なら、インターフェースの課題は達成している。`Character` の `if (type == "ice")` という分岐で文章を出していると、ここで `Character` を書き換えることになり、設計が課題の意図とずれる。

---

## 13. 落とし穴カタログ

| # | 症状 | 原因 |
|---|------|------|
| 1 | 基底ポインタだと全員が同じ鳴き声 | `makeSound` が virtual ではない。または派生のシグネチャが違う（const 落ち） |
| 2 | 具体的な `Dog d; d.makeSound();` は正しいのに、課題の main だけ違う | 1 と同じ。具体型からの呼び出しは virtual 不要なのでバグが隠れる |
| 3 | `delete` のログが `~Animal` だけ | デストラクタが virtual ではない。未定義動作 |
| 4 | `Dog` を消すと LeakSanitizer / valgrind が `Brain` を指摘 | `~Dog` で `delete brain` していない |
| 5 | コピーのあと二重解放で落ちる、またはアイデアが共有される | コピーが `brain(other.brain)` |
| 6 | コピーした犬の `getType()` が空 | コピーコンストラクタが `: Animal(other)` を呼んでいない |
| 7 | `-Werror` で initializer list order | 初期化子リストの並びが、基底・宣言順と違う |
| 8 | ex02 で `new Dog()` までエラー | `Dog` が `makeSound` を実装していない。`Dog` も抽象のまま |
| 9 | `createMateria("ice")` がいつも 0 | `type` に `"ice"` が入っていない（`type = type` の影）。または大文字 `"Ice"` で覚えた |
| 10 | 提示 main が valgrind で Ice/Cure をリーク | `learnMateria` が clone しただけで引数を所有していない |
| 11 | unequip のあとに手元のポインタを `delete` すると不正 | `equip` が別の clone を格納し、引数を消した。または unequip が delete した |
| 12 | キャラクターのコピーを両方破棄すると落ちる | スロットを共有している。`clone()` していない |
| 13 | `c = c` のあと壊れる | 自己代入で自分のマテリアを delete してから clone している |
| 14 | 代入した側の古い氷がリーク | 代入で旧スロットを delete していない |
| 15 | ヘッダを 2 回 include すると再定義エラー | インクルードガードが無い |
| 16 | `Animal.hpp` を単体で include すると `std::string` が不明 | ヘッダが `<string>` を自分で include していない |
| 17 | 循環 include で不完全型のエラー | `AMateria.hpp` と `ICharacter.hpp` が互いを include している |
| 18 | `using namespace std` や `friend` で大幅減点 | 課題の一般規則。明示許可が無いので使わない |
| 19 | `std::vector` で床やインベントリを作って大幅減点 | STL コンテナは Module 08 まで禁止 |
| 20 | ex03 の出力に構築メッセージが混ざる | ex00 の習慣のまま印字している |
| 21 | `delete[] animals` で壊れる | ポインタ配列の要素は個別の `new`。`delete[]` は `new[]` と対 |
| 22 | スタックの `Dog` を基底ポインタで `delete` | `new` していない |
| 23 | `idx` を `unsigned` にして負の unequip が壊れる | 負数が巨大な正になる |
| 24 | 課題の main の `const Animal*` がコンパイルできない | `makeSound` / `getType` が const ではない |

---

## 14. テスト戦略

課題は全演習で「提示以上のテストを実装して提出せよ」と書いている。評価者は main を読む。表示が何を証明しているか、コメント 1 行で足りる。

### 14.1 ex00

- 課題の main と同じ並び（`getType`、猫、犬、Animal）
- その 3 匹を `delete` し、派生→基底のメッセージ順
- `WrongAnimal*` 経由の `WrongCat` は基底の鳴き声
- 具体的な `WrongCat` 変数は `WrongCat` の鳴き声
- `Dog` のコピーの `getType()` が `"Dog"`
- 値スライス `Animal sliced = dog; sliced.makeSound();` は Animal の鳴き声（ex02 ではこの行は削除）

### 14.2 ex01

- 提示の `new Dog` / `new Cat` / `delete`
- 偶数長の `Animal*` 配列。前半 `Dog`、後半 `Cat`。ループで `delete`
- ログに `Brain` の破棄が、各犬・各猫につき 1 回
- コピー構築: アイデアを変えても元は変わらない。`getBrain()` のアドレスが違う
- 代入: 同じことを代入後のオブジェクトで
- `Cat` でも深いコピーを 1 組
- 自己代入 `d = d` のあと、アイデアが残り、破棄が落ちない

### 14.3 ex02

- `new Animal()` がソースに残っていないこと（残っていると翻訳できないので、テストというよりビルドが証明になる）
- ex01 の犬猫テストが同じ結果

### 14.4 ex03

提示 main は、クラスが沈黙していれば 2 行になる。追加テストは表示が増えてよい。最低限、次を自分の main に入れる。

| テスト | 観測 |
|--------|------|
| 未知タイプ `createMateria("fire")` | 0。そのポインタを equip しない |
| インベントリ満杯で 5 個目の equip | 何も増えない。5 個目のポインタは自分で `delete` |
| `use(-1)` / `use(4)` / 空スロット | 出力無し、クラッシュ無し |
| `unequip` したスロットを `use` | 出力無し |
| unequip 前に保存したポインタをあとで `delete` | リーク無し、二重解放無し |
| unequip(0) のあと equip | スロット 0 に入る |
| `Character` のコピー構築後、両方 `use` | 同じ文章が出る |
| コピー側の装備を unequip しても元は `use` できる | 深いコピー |
| 代入で、代入先が持っていた別マテリアがリークしない | 古い氷を消してから治療を clone |
| 自己代入 | 装備が残る |
| `learn` を 5 回 `new` で呼ぶ | 5 個目を捨ててもリークしない（自分の契約どおり） |
| `MateriaSource` のコピーから `createMateria` | コピー側だけ破棄しても、元からまだ作れる |

`equip(0)` は何もしない、も 1 行あると安心。

### 14.5 リークの見方

評価環境は Linux。`valgrind --leak-check=full` が定番。

Windows ネイティブには valgrind が無い。WSL か、コンパイラのサニタイザをローカルだけで使う。

```text
c++ -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address,undefined *.cpp
```

サニタイザのフラグは提出用 Makefile に入れない。学校の環境差と、課題が指定したフラグ以外を混ぜないため。ローカルの別ターゲットか、手打ちでよい。

「definitely lost」が 0 であること。二重解放はサニタイザが即時に止める。valgrind で「まだ到達可能」と出るブロックは、静的領域にアドレスが残っているときに出ることがある。`new` の対が自分のデストラクタにあるかを、第12章の表で数える方が先。

---

## 15. ビルドとツール

C モジュールの Makefile 規則が適用される、と課題にある。ワイルドカードは Norm の Makefile 章で禁止。ソースファイルは列挙する。

```make
NAME    = animal
CXX     = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS    = main.cpp Animal.cpp Dog.cpp Cat.cpp WrongAnimal.cpp WrongCat.cpp
OBJS    = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
```

| 規則 | 理由 |
|------|------|
| `all` / `clean` / `fclean` / `re` / `$(NAME)` | 必須ターゲット |
| 変更が無ければ再リンクしない | `$(NAME)` がオブジェクトに依存していれば、make が判断する |
| `.PHONY` | 同名ファイルが無いように |
| ソースをベタ書き | ワイルドカード禁止 |
| `-std=c++98` をフラグに含める | 課題は「このフラグを足してもコンパイルできること」。最初から付けておくと、C++11 を誤って使った瞬間に気づく |

バイナリ名の指定は課題に無い。`ex00` は `animal`、`ex03` は `materia` のように演習が分かればよい。`ex00` の成果物を `.gitignore` するなら、モジュール 02/03 と同様に `*.o` とバイナリ名。

C++11 以降で、この課題の最中につい出てしまうもの:

| 使わない | 代わり |
|----------|--------|
| `nullptr` | `0` |
| `override` / `final` | シグネチャを目で合わせる。ローカル警告は `-Woverloaded-virtual` |
| `= default` / `= delete` | 関数を自分で書く。コピーを禁じるなら private 宣言して定義しない（今回は深いコピーを書くので不要） |
| `auto` | 型を書く |
| 範囲 for | 添字の for |
| `std::vector` / `std::array` | 生配列 |
| ラムダ | 普通の関数 |
| `enum class` | 必要なら従来の enum。スロット数は `static const int` で足りる |

`std::string` と `<iostream>` は課題が使っている。禁止されているのはコンテナと `<algorithm>` と、`printf` 族・`malloc` 族・`free`。`new` / `delete` は必須。

---

## 16. 知っておくと差がつくこと

実装に必須ではない。評価の会話と、この先のモジュールで効く。

### 16.1 非 virtual 関数は実体のデータを触れる

第3章の繰り返しを 1 文で。`WrongAnimal*` 経由でも `type` は `WrongCat` が書いた値。切り替わるのは関数のどれを実行するか。オブジェクトが基底に化けるわけではない。化けるのはスライスと、非 virtual デストラクタの `delete`（そして後者は未定義動作）。

### 16.2 構築中の virtual 呼び出し

第4.4章。基底コンストラクタから virtual を呼ぶと、派生のオーバーライドには届かない。初期化は派生コンストラクタの本体か、派生の初期化子リストで行う。`type = "Dog"` を `Animal` のコンストラクタから virtual 関数で決めさせない。

### 16.3 抽象クラスはスライスをコンパイルエラーにする

ex02 の副産物。`void f(Animal a)` や `Animal a = dog` は書けなくなる。Module 03 で「値では渡さない」と覚えた規則が、型システム側でも守られる。ポインタと参照は残る。残った経路で `delete` するなら virtual デストラクタ、というセット。

### 16.4 clone は Prototype

GoF の Prototype は「見本のオブジェクトに、自分の複製の作り方を聞く」パターン。`AMateria::clone` がそれ。`MateriaSource` は見本をタイプ名で引けるように保管する工場。文字列 `"ice"` から具象クラスを `if` で new する工場も書けるが、その `if` は新しいマテリアのたびに工場を修正する。見本に `clone` させると、工場はタイプ文字列の一致だけを知る。

`Character` のコピーが `clone()` を呼ぶのは、スロットの動的な型が `Ice` か `Cure` か `Fire` かを `Character` が知らないから。ex01 の `Dog` コピーは、コピーコンストラクタの時点で `Dog` と分かっているので `new Brain` で足りた。

### 16.5 リスコフの置換原則

S が T のサブタイプなら、T のところに S を置いても、プログラムの期待が壊れない、という原則（Liskov）。

`Dog` を `Animal*` に入れて `makeSound()` するのは、この原則に沿った使い方。呼び出し側の期待は「その動物の鳴き声がする」であり、犬の鳴き声はその期待の範囲。

`WrongCat` を `WrongAnimal*` に入れると、呼び出し側が `WrongAnimal` の鳴き声を受け取る。派生独自の鳴き声は、このポインタ経由では存在しない。is-a の見た目だけで、振る舞いの置換は成立していない。課題がその対比を見せろと言っているのは、この差。

正方形と長方形の話（幅を変えると高さも変わる正方形は、独立に幅と高さを変えられる長方形のサブタイプとしては壊れる）は Module 03 のノートにある。同じ原則。

### 16.6 インターフェースの多重継承

Module 03 のダイヤモンドは、**データと実装を 2 経路で継承した**ときに基底が 2 個になる問題だった。データ無しのインターフェースを 2 つ継承しても、共有すべきメンバ変数が無いので、あの形のダイヤモンドは起きにくい。関数シグネチャが衝突したら、派生が 1 つの実装を書けば両方の純粋仮想を満たせる。

今回 `Character` が継承するのは `ICharacter` 1 つ。多重継承は書かない。キーワード `virtual` を継承指定に付けない。

2 番目以降の基底はオブジェクト先頭に無く、vptr がもう 1 本増える。仮想呼び出しの前に `this` をずらすサンクが要る。`delete` のポインタを先頭へ戻さないと `free` が塊を壊す。手順は [4.12](#412-基底が先頭に無いときthis-をずらしてから飛ぶ)。このモジュールの単一継承では変位は 0 である。

### 16.7 private な純粋仮想関数

純粋仮想関数は private にもできる。外からは基底の public な非 virtual 関数だけを呼び、その中から private virtual を呼ぶ、という Non-Virtual Interface という形がある。呼び出し規約を基底が固定し、中身だけ派生が差し替える。この課題の `makeSound` と `use` は、課題の例が外から直接呼ぶので public のままにする。

### 16.8 RTTI

`dynamic_cast` と `typeid` は、virtual 関数を 1 つ以上持つクラス（多態的クラス）で意味を持つ。`dynamic_cast<Dog*>(animal)` で犬かどうかを分岐するのは、`makeSound` を virtual にした意味を捨てている。種類ごとの処理は仮想関数に置く。キャストの詳細は Module 06。

`typeid(x).name()` の文字列は処理系定義のマングル名。テストの期待値にしない。

### 16.9 オブジェクトのサイズを一度見る

```cpp
std::cout << sizeof(Animal) << std::endl;
std::cout << sizeof(Dog) << std::endl;
```

virtual を外したときと付けたときで、ポインタ 1 本分だけ差が出ることを確認する。`std::string` のサイズはライブラリの版で変わるので、ノートに固定値を書かない。欄の並びは `c++ -std=c++98 -fdump-class-hierarchy` で見る（[4.11](#411-itanium-abi-では-vptr-が表の途中を指す)）。

### 16.10 ヘッダに書いたメンバ関数はインライン定義

クラス定義の中の `{}` は、ヘッダに実装を書いたことになる。課題の `~ICharacter() {}` 以外はやらない。テンプレートの実装をヘッダに置く例外は Module 07。

### 16.11 開放と例外

`new` は失敗すると `std::bad_alloc` を投げる。このモジュールでは捕捉しない（Module 05）。失敗しない前提で、所有者が 1 人になる経路を先に正しくする。

---

## 17. defense 想定問答

声に出して、各問に 2 文で答えられるかを見る。

### 基本

1. **静的な型と動的な型は何か。** `Animal* p = new Dog();` を使って。
2. **`getType()` は virtual でなくても `"Dog"` を返せる。なぜ `makeSound()` は virtual が要るのか。**
3. **`WrongCat` を `WrongAnimal*` で `makeSound` すると誰が鳴くか。同じオブジェクトを `WrongCat` 変数で呼ぶと誰が鳴くか。**
4. **オーバーライドと名前隠蔽の違いは何か。**
5. **vtable と vptr は何か。オブジェクトごとに vtable はあるか。**
6. **`virtual` を関数に付けた場合と、継承に付けた場合の違いは何か。**
7. **なぜ基底コンストラクタの中の virtual 呼び出しは派生に届かないのか。**
8. **`const` を忘れた `Dog::makeSound()` はどうなるか。コンパイルは通るか。**
9. **`delete` するポインタの型が基底のとき、何が必要か。無いと規格上何と呼ばれるか。**
10. **`~Dog` と `~Animal` と `~Brain` の順序は。ポインタメンバは指先を自動で delete するか。**

### ex01

11. **浅いコピーとはどの 1 行か。そのあと何が起きるか。**
12. **深いコピーのあと、アイデアを書き換えるテストは何を 2 つ比較するか。**（中身と、`Brain*` のアドレス）
13. **なぜ `Brain` を値メンバにしないのか。**（課題が `new` とポインタを指定している。値なら `std::string` のコピーが自動で深く、この演習にならない）
14. **`Animal` の配列ではなく `Animal*` の配列なのはなぜか。**
15. **`delete` と `delete[]` のどちらを、どのポインタに使うか。**
16. **コピーコンストラクタで `: Animal(other)` を省くと `type` はどうなるか。**
17. **自己代入 `d = d` で、先に `delete brain` すると何が起きるか。**

### ex02

18. **抽象クラスの定義は何か。**（純粋仮想関数を 1 つ以上持つ）
19. **protected コンストラクタだけでは、なぜ課題の答えとして弱いか。**
20. **抽象クラスへのポインタは作れるか。値は作れるか。**
21. **`Dog` が `makeSound` を書き忘れるとどうなるか。**
22. **デストラクタを純粋仮想にしたとき、cpp に本体は要るか。なぜか。**

### ex03

23. **インターフェースと `AMateria` の違いは何か。**
24. **なぜ `Character` は `Ice` を include しなくてよいか。**
25. **`equip` は clone するか、ポインタをそのまま持つか。unequip のテストとどう繋がるか。**
26. **`unequip` のあと誰が delete するか。**
27. **`learnMateria(new Ice())` の Ice を誰が delete するか。なぜその読みにしたか。**
28. **`createMateria` が返すオブジェクトと、覚えたテンプレートは同一か。**
29. **未知のタイプの戻り値は何か。C++98 で `nullptr` と書かない理由は。**
30. **代入が type をコピーしない理由を、Ice と Cure を使って。**
31. **キャラクターの代入で、古いマテリアを消すタイミングは。自己代入はなぜ先に返すか。**
32. **提示 main の `use(0, *bob)` が氷の文になるのは、関数のどの連鎖か。**
33. **`ICharacter` のデストラクタが virtual な理由は。`AMateria` もか。**
34. **満杯の `equip` と、満杯の `learnMateria` で、失敗したポインタの扱いを同じにしてよいか。**（提示コードの所有の残り方が違う）
35. **`Fire` を足すとき変更するファイルはどれで、変更しないファイルはどれか。**
36. **前方宣言で足りるのはどの関係で、include が要るのはどのファイルか。**
37. **ex03 の構築時にメッセージを出さない理由は。**

### 発展

38. **3 種類のポリモーフィズムを、モジュール番号付きで。**
39. **virtual にするとオブジェクトサイズと呼び出しに何が起きるか。**
40. **スライシングしたあとの virtual 呼び出しは誰になるか。なぜか。**
41. **`dynamic_cast` で犬かどうかを分岐するのは、このモジュールの意図とどうずれるか。**
42. **Rule of Three はどのクラスに適用されたか。**
43. **公開 virtual デストラクタと、protected 非 virtual デストラクタの使い分けは。**
44. **共変戻り値型とは何か。`clone` で使えるか。**
45. **`p->makeSound()` は、x86-64 ではどのレジスタに `this` が入り、何が間接 `call` になるか。**
46. **vtable はプロセスのどの領域にあり、オブジェクト本体はどこか。**
47. **virtual で無い `delete` のあと、犬の `malloc` 塊と `Brain` の塊はそれぞれどうなりやすいか。C++98 の `operator delete` はサイズを受け取るか。**
48. **`Character` が `Ice` を include しないことと、Linux の `file_operations` は何が同じか。**

### 47 の答え

単一継承で `Animal` が先頭なら、`Animal*` の値は `new Dog` が返したアドレスと同じである。C++98 の `operator delete` は `void*` だけで、glibc の `free` は塊ヘッダのサイズを見る。犬の塊は `free` されることが多い。`~Dog` が走らないので、別の `malloc` である `Brain` が残る。これは未定義動作の観測であり、保証ではない。ヒープを壊す典型は、基底が先頭に無いのに `free` へ内部ポインタを渡したときである。

### 8 の答え

基底が `virtual void makeSound() const` で、派生が `void makeSound()` のとき、派生の関数はオーバーライドではない。`const Animal*` から呼べる `makeSound` は基底のものだけなので、基底の鳴き声になる。コンパイルは通る。C++98 には `override` が無く、コンパイラが誤りとして止めない。

### 26 の答え

`unequip` はスロットを 0 にするだけで `delete` しない。`delete` するのは、unequip の前からそのアドレスを持っていた呼び出し側。持っていなければリークする。だから課題は「先にアドレスを保存せよ」と書いている。

### 32 の答え

`me` の静的な型は `ICharacter*`、動的な型は `Character`。`use` は純粋仮想なので `Character::use(0, bob)` に着地する。スロット 0 には `createMateria("ice")` が作った `Ice` がある。`AMateria::use` は virtual なので `Ice::use` が走り、`bob.getName()` が `"bob"` を返す。文章は `* shoots an ice bolt at bob *`。

---

## 18. 用語集

| 用語 | 意味 |
|------|------|
| ポリモーフィズム / 多態性 | 1 つの名前が複数の型で意味を持つこと。このモジュールはサブタイプの方 |
| 静的な型 | 式に書いてある型 |
| 動的な型 | 実行時のオブジェクトの型 |
| 静的束縛 | 呼び先がコンパイル時に決まること |
| 動的束縛 / 動的ディスパッチ | 呼び先が実行時に決まること |
| virtual 関数 | 動的ディスパッチされるメンバ関数 |
| オーバーライド | 基底の virtual 関数を、同じシグネチャで派生が置き換えること |
| 名前隠蔽 | 非 virtual、またはシグネチャが違う関数が、基底の同名を隠すこと |
| vptr | オブジェクトが持つ、vtable への隠しポインタ |
| vtable | クラスごとの関数アドレス表。コンパイラの実装方式であり、規格の用語ではない |
| 純粋仮想関数 | `= 0` の virtual 関数 |
| 抽象クラス | 純粋仮想関数を 1 つ以上持つクラス。インスタンス化できない |
| インターフェース | データが無く、操作が純粋仮想なクラス、という C++ の慣習 |
| 具象クラス | インスタンス化できるクラス |
| OCF | デフォルト構築、コピー構築、コピー代入、破棄 |
| Rule of Three | 破棄・コピー構築・コピー代入のどれかを自分で書いたら、3 つ書く |
| 浅いコピー | ポインタ値だけをコピーし、指先を共有すること |
| 深いコピー | 指先のオブジェクトも別に作って中身をコピーすること |
| 所有権 | `delete` する責任が誰にあるか |
| スライシング | 派生を基底の値にコピーして、派生部分が失われること |
| 未定義動作 | 規格が振る舞いを定めないこと。動いたように見えても正しいとは限らない |
| 前方宣言 | クラス名だけを先に宣言すること。ポインタと参照の宣言に使える |
| 共変戻り値型 | オーバーライドの戻り値を、基底の戻り値の派生ポインタ／参照にしてよい、という規則 |
| RTTI | 実行時型情報。`dynamic_cast` / `typeid`。vtable の手前の typeinfo を使う |
| サンク（thunk） | 仮想呼び出しの前に `this` をずらす短い関数。2 番目以降の基底から呼ぶときに要る |
| 削除用デストラクタ | `delete` が飛ぶ先。破棄のあと `operator delete` を呼ぶ。Itanium ABI の用語 |
| 完全デストラクタ | 基底の破棄から呼ばれるデストラクタ。メモリは解放しない |
| `.rodata` | 読み取り専用データ。vtable と文字列リテラルが置かれる |
| `.text` | 機械語が置かれる領域。`Dog::makeSound` の本体はここ |
| 間接呼び出し | 飛び先アドレスをレジスタやメモリから読む `call`。仮想関数はこれになる |
| Prototype | 見本オブジェクトに複製を頼む設計 |
| LSP | リスコフの置換原則。サブタイプは基底の期待を壊さない |

---

## 19. 参考文献

| 分類 | 文献 | このモジュールでの該当 |
|------|------|------------------------|
| 設計 | Scott Meyers, *Effective C++* 第3版 | Item 7（多態的基底の virtual デストラクタ）、Item 9（構築・破棄中に virtual を呼ばない）、Item 34（インターフェース継承と実装継承）、Item 36（非 virtual を再定義しない） |
| 設計 | Scott Meyers, *More Effective C++* | Item 33（末端でないクラスは抽象に） |
| 設計 | Sutter & Alexandrescu, *C++ Coding Standards* | Item 50（デストラクタは public virtual か protected 非 virtual） |
| 内部 | Stanley Lippman, *Inside the C++ Object Model* | vptr / vtable の配置 |
| 規格 | ISO/IEC 14882:1998 | [class.virtual]、[class.abstract]、[class.cdtor]、[expr.delete]、[class.copy] |
| ABI | Itanium C++ ABI | gcc / clang の vtable。vptr は最初の仮想関数を指し、手前に offset-to-top と typeinfo。削除用デストラクタ |
| ABI | System V AMD64 ABI | x86-64 では `this` が `rdi`。仮想呼び出しは vptr を読んでからの間接 `call` |
| OS | Linux `file_operations`（`include/linux/fs.h`） | カーネルの手書き vtable。VFS が具象ファイルシステムを include しない構造は、`Character` と `Ice` と同じ |
| 原則 | Barbara Liskov, *Data Abstraction and Hierarchy*（1987） | 置換原則 |
| パターン | GoF, *Design Patterns* | Prototype。Factory Method との対比 |
| オンライン | [cppreference — virtual function](https://en.cppreference.com/w/cpp/language/virtual) | virtual / 純粋仮想 / 共変戻り値 |
| オンライン | [cppreference — abstract class](https://en.cppreference.com/w/cpp/language/abstract_class) | 抽象クラスにできることとできないこと |
| オンライン | [isocpp.org C++ FAQ — Inheritance](https://isocpp.org/wiki/faq/strange-inheritance) | 構築中の virtual 呼び出し |

規格の条番号は版で見出し名が違う。上の `[class....]` はタグで、C++98 本文の該当節に対応する。

---

## 20. 次モジュールへの接続

| モジュール | テーマ | Module 04 のどこが効くか |
|------------|--------|--------------------------|
| **05** | 例外 | `new` の失敗（`std::bad_alloc`）と、途中で失敗したときの所有権。今は「所有者を 1 人にする」まで。例外安全なコピーはここで本格化する |
| **06** | キャスト | `dynamic_cast` は多態的クラスにしか効かない。仮想関数で足りる場面にキャストを使わない、という判断が先にある |
| **07** | テンプレート | 3 つ目のポリモーフィズム。`clone` の戻り値を型安全にする話は、テンプレートと組み合わせたときに再登場する（共変戻り値で足りる範囲は今回で終わっている） |
| **08** | STL コンテナ | `std::vector<Animal*>` のように、ポインタを入れてスライスを避ける。コンテナが中身をコピーするので、値で `Animal` を入れるとまたスライスする。所有ポインタをコンテナに入れたときの `delete` 責任は、今回手で書いたループと同じ問題 |
| **09** | アルゴリズム | 関数オブジェクト。virtual 関数で差し替えていた戦略を、テンプレート引数で差し替える選択肢が増える |

Module 03 からの対応表を、完成形で書き直すとこうなる。

| Module 03 で止めていたこと | Module 04 での答え |
|----------------------------|--------------------|
| `ClapTrap* p = &scav; p->attack()` は基底の関数 | 関数を virtual にすると派生の関数になる |
| 基底ポインタの `delete` は未定義動作、と知っていた | `Animal` / `AMateria` / `ICharacter` のデストラクタを public virtual にした |
| スライシングに注意していた | 抽象クラスにして、値でのスライスを翻訳エラーにした |
| コピーは基底部分を明示的に呼ぶ | それに加えて、自分で `new` したものを深くコピーする |

---

## 21. 提出前チェックリスト

### 全演習共通

- [ ] `c++ -Wall -Wextra -Werror -std=c++98` で警告ゼロ
- [ ] ヘッダにインクルードガードがある
- [ ] 課題が示したインターフェースの空デストラクタ以外、関数本体をヘッダに書いていない
- [ ] 各ヘッダが単体で include できる
- [ ] `using namespace` が無い
- [ ] `friend` が無い
- [ ] `std::vector` などのコンテナと `<algorithm>` が無い
- [ ] `printf` / `malloc` / `free` / `nullptr` / `override` が無い
- [ ] メッセージは `std::cout` で、改行で終わる
- [ ] Makefile に `all` `clean` `fclean` `re` `$(NAME)` と `.PHONY` があり、ソースを列挙している
- [ ] `make` を 2 回実行しても再リンクしない
- [ ] ディレクトリ名が `ex00` 〜 `ex03`、ファイル名がクラス名と一致する
- [ ] リークが無い（valgrind かサニタイザ）

### ex00

- [ ] `Animal` の `type` が protected な `std::string`
- [ ] `Dog` の type が `"Dog"`、`Cat` の type が `"Cat"`
- [ ] `makeSound` と `getType` が const
- [ ] `makeSound` が virtual で、猫と犬と `Animal` の文が全部違う
- [ ] デストラクタが virtual で、クラスごとにメッセージが違う
- [ ] `WrongAnimal::makeSound` は virtual ではない
- [ ] `WrongAnimal*` 経由の `WrongCat` は `WrongAnimal` の鳴き声
- [ ] 提出 main が `delete` している
- [ ] OCF 4 関数がある

### ex01

- [ ] `Brain` が `ideas` という `std::string[100]` を持つ
- [ ] `Dog` と `Cat` だけが `Brain*` を持ち、構築で `new Brain()`、破棄で `delete`
- [ ] `Animal*` の配列の半分が犬、半分が猫で、`Animal*` として `delete` している
- [ ] 破棄ログが派生 → Brain → 基底の順
- [ ] コピー構築と代入の両方が深いコピーで、テストが中身とアドレスを見ている
- [ ] コピーコンストラクタが基底を `Animal(other)` で初期化している
- [ ] 自己代入しても壊れない

### ex02

- [ ] `Animal::makeSound` が純粋仮想
- [ ] `new Animal()` と `Animal` の値がソースに無い
- [ ] `Dog` と `Cat` はこれまでどおり生成でき、ex01 のテストが通る
- [ ] 改名した場合、ファイル名もクラス名と一致している

### ex03

- [ ] `Ice` の type が `"ice"`、`Cure` の type が `"cure"`
- [ ] `use` の文が空白ごと課題と一致し、名前に山括弧が無い
- [ ] `AMateria::operator=` が type をコピーしない
- [ ] `clone()` が同じ具象クラスの `new` を返す
- [ ] `~AMateria` と `~ICharacter` と `~IMateriaSource` が virtual
- [ ] `equip` は最初の空きスロットに同じポインタを入れ、満杯でも delete しない
- [ ] `unequip` は delete しない
- [ ] 範囲外と空スロットの `use` / `unequip` は何もしない
- [ ] `Character` のコピーは `clone()` で深く、代入は旧マテリアを先に delete する
- [ ] `learnMateria` の所有権を説明でき、提示 main がリークしない
- [ ] `createMateria` は未知タイプで 0、既知タイプでテンプレートとは別の個体
- [ ] 提示 main と同じ手順のとき、クラスが余計な行を出さない
- [ ] `Character` が `Ice.hpp` に依存していない
- [ ] 追加テストが、満杯・unequip・深いコピー・未知タイプを含んでいる

---

## 22. 関連ファイル

| ファイル | 用途 |
|----------|------|
| `04/knowledge.md` | 本文書 |
| `03/knowledge.md` | 継承、構築順、スライシング、名前隠蔽。本文書の直前 |
| `02/knowledge.md` | OCF、アドホック多態性、`std::swap` がこの課程では使いにくい理由 |

実装ディレクトリ `04/ex00` 〜 `04/ex03` は、このノートの契約を自分の手で満たす場所。ノートのコード片は、その契約がどういう形になるかの見本であって、提出物そのものではない。
