# RPN (逆ポーランド記法) 計算機

通常の数式（中置記法）をRPN（逆ポーランド記法）に変換し、計算するC++プログラムです。

## 概要

このプログラムは以下の機能を提供します：

1. **中置記法 → RPN変換**: `(1 + 2) * 3` → `1 2 + 3 *`
2. **RPN → 中置記法変換**: `1 2 + 3 *` → `((1 + 2) * 3)`
3. **RPN計算**: `1 2 + 3 *` → `9`

## 動作要件

- **C++17以上**に対応したコンパイラ（g++, clang++など）
- **Google Test**（テスト実行時のみ必要）
- 使用している標準ライブラリ:
  - `<iostream>`, `<string>`, `<stack>`, `<vector>`
  - `<cctype>`, `<cmath>`
  - `<unordered_map>`, `<unordered_set>`
  - `<functional>`, `<algorithm>`

## ファイル構成

```
RPN/
├── CMakeLists.txt      # メインのCMake設定
├── README.md       # このファイル
├── src/
│   ├── librpn.hpp         # RPN計算ライブラリのヘッダ（API定義）
│   ├── librpn.cpp         # RPN計算ライブラリの実装
│   └── main.cpp        # デモプログラム
└── test/
    ├── CMakeLists.txt  # テスト用CMake設定
    └── rpn_test.cpp    # Google Testによるユニットテスト
```

## クイックスタート

### ビルド

```bash
g++ -o rpn src/librpn.cpp src/main.cpp -std=c++17
```

### 実行

```bash
./rpn
```

### テスト

CMakeを使用してビルドすると、実行ファイル（`a.out`）とテスト（`rpn_test`）が同時に生成されます。

```bash
# ビルドディレクトリを作成
mkdir build && cd build

# CMakeでビルド（実行ファイルとテストを同時にビルド）
cmake ..
make

# テストを実行
ctest --output-on-failure

# または直接テスト実行
./rpn_test
```

> **Note**: Google Testがシステムにインストールされている必要があります。

### 使用例

```cpp
#include "librpn.hpp"

// 中置記法 → RPN → 計算
std::string infix = "sqrt(16) + 2 ^ 3";
std::string rpn = librpn::infixToRPN(infix);    // "16 sqrt 2 3 ^ +"
double result = librpn::calculateRPN(rpn);       // 12.0

// RPN → 中置記法
std::string rpn2 = "1 2 + 3 *";
std::string infix2 = librpn::rpnToInfix(rpn2);  // "((1 + 2) * 3)"
```

> **Note**: すべての関数は `librpn` 名前空間で定義されています。
> `using namespace librpn;` を使用すれば接頭辞を省略できます。

### 実行結果

```text
=== 通常の数式からRPNへの変換 ===
Infix: (1 + 2) * (3 + 4)
RPN: 1 2 + 3 4 + *
Result: 21

Infix: sqrt(16) + 2
RPN: 16 sqrt 2 +
Result: 6

Infix: 2 ^ 3 ^ 2
RPN: 2 3 2 ^ ^
Result: 512

Infix: sqrt(abs(-9)) * cos(0) + log(exp(1))
RPN: -9 abs sqrt 0 cos * 1 exp log +
Result: 4

=== Unicode演算子・記号の例 ===
Infix: 3 × 4 ÷ 2
RPN: 3 4 × 2 ÷
Result: 6

Infix: √(16) + π
RPN: 16 √ π +
Result: 7.14159

Infix: 2 × π × 3 (円周 2πr, r=3)
RPN: 2 π × 3 ×
Result: 18.8496

=== RPNから中置記法への変換 ===
RPN: 1 2 + 3 4 + *
Infix: ((1 + 2) * (3 + 4))
Result: 21

RPN: 16 sqrt 2 +
Infix: (sqrt(16) + 2)
Result: 6

RPN: 2 3 2 ^ ^
Infix: (2 ^ (3 ^ 2))
Result: 512

RPN: 3 4 × 2 ÷
Infix: ((3 × 4) ÷ 2)
Result: 6

=== 二項関数の例 ===
Infix: pow(2, 10)
RPN: 2 10 pow
Result: 1024
Back to Infix: pow(2, 10)

Infix: max(3, 7) + min(10, 5)
RPN: 3 7 max 10 5 min +
Result: 12
Back to Infix: (max(3, 7) + min(10, 5))

Infix: atan2(1, 1)
RPN: 1 1 atan2
Result: 0.785398
Back to Infix: atan2(1, 1)

Infix: mod(17, 5)
RPN: 17 5 mod
Result: 2
Back to Infix: mod(17, 5)

Infix: pow(sqrt(16), 3)
RPN: 16 sqrt 3 pow
Result: 64
Back to Infix: pow(sqrt(16), 3)

Infix: max(pow(2, 3), min(10, 5))
RPN: 2 3 pow 10 5 min max
Result: 8
Back to Infix: max(pow(2, 3), min(10, 5))

=== 統計関数の例（HP電卓方式） ===
RPN: { 1 2 3 4 5 } sum
Result (sum): 15

RPN: { 2 4 6 8 10 } mean
Result (mean): 6

RPN: { 3 1 4 1 5 9 2 6 } median
Result (median): 3.5

RPN: { 2 4 6 8 } stddev
Result (stddev): 2.23607

RPN: { 2 4 6 8 } var
Result (var): 5

RPN: { 3 1 4 1 5 9 2 6 } lmax
Result (lmax): 9

RPN: { 3 1 4 1 5 9 2 6 } lmin
Result (lmin): 1

RPN: { 1 2 3 4 5 } product
Result (product): 120

=== 統計関数と演算の組み合わせ ===
RPN: { 2 4 6 8 } mean { 2 4 6 8 } stddev +
Result (mean + stddev): 7.23607

=== 中置記法からリスト統計関数への変換 ===
Infix: { 10, 20, 30, 40, 50 } mean
RPN: { 10 20 30 40 50 } mean
Result: 30
```

## 対応する演算子・関数・定数

### 演算子

#### ASCII演算子

| 演算子 | 説明 | 優先順位 | 結合性 |
|--------|------|----------|--------|
| `+` | 加算 | 1 | 左 |
| `-` | 減算 | 1 | 左 |
| `*` | 乗算 | 2 | 左 |
| `/` | 除算 | 2 | 左 |
| `%` | 剰余 | 2 | 左 |
| `^` | べき乗 | 3 | 右 |

#### Unicode演算子

| 演算子 | Unicode | 説明 | 優先順位 | 結合性 |
|--------|---------|------|----------|--------|
| `×` | U+00D7 | 乗算 | 2 | 左 |
| `÷` | U+00F7 | 除算 | 2 | 左 |
| `·` | U+00B7 | 乗算（中点） | 2 | 左 |

### 関数

#### 単項関数（ASCII）

| 関数 | 説明 |
|------|------|
| `sqrt(x)` | 平方根 |
| `sin(x)` | 正弦（ラジアン） |
| `cos(x)` | 余弦（ラジアン） |
| `tan(x)` | 正接（ラジアン） |
| `log(x)` | 自然対数 |
| `ln(x)` | 自然対数（別名） |
| `log10(x)` | 常用対数 |
| `abs(x)` | 絶対値 |
| `exp(x)` | 指数関数 e^x |
| `floor(x)` | 切り捨て |
| `ceil(x)` | 切り上げ |

#### 単項関数（Unicode）

| 記号 | Unicode | 説明 |
|------|---------|------|
| `√` | U+221A | 平方根 |

#### 二項関数

| 関数 | 説明 | RPNでの表現 |
|------|------|-------------|
| `pow(x, y)` | x の y 乗 | `x y pow` |
| `max(x, y)` | x と y の大きい方 | `x y max` |
| `min(x, y)` | x と y の小さい方 | `x y min` |
| `atan2(y, x)` | y/x のアークタンジェント | `y x atan2` |
| `mod(x, y)` | x を y で割った余り | `x y mod` |

#### リスト関数（統計関数）- HP電卓方式

HP電卓の方式に基づき、`{ }` でリストを表現し、統計関数を適用します。

| 関数 | 説明 | RPNでの表現 | 例 |
|------|------|-------------|-----|
| `sum` / `ΣLIST` | 合計 | `{ a b c } sum` | `{ 1 2 3 } sum` → 6 |
| `product` / `ΠLIST` | 積 | `{ a b c } product` | `{ 2 3 4 } product` → 24 |
| `mean` | 平均 | `{ a b c } mean` | `{ 2 4 6 } mean` → 4 |
| `median` | 中央値 | `{ a b c } median` | `{ 1 3 5 } median` → 3 |
| `var` | 母分散 | `{ a b c } var` | `{ 2 4 6 8 } var` → 5 |
| `svar` | 標本分散 | `{ a b c } svar` | `{ 2 4 6 8 } svar` → 6.67 |
| `stddev` | 母標準偏差 | `{ a b c } stddev` | `{ 2 4 6 8 } stddev` → 2.236 |
| `sstddev` | 標本標準偏差 | `{ a b c } sstddev` | `{ 2 4 6 8 } sstddev` → 2.582 |
| `lmax` | 最大値 | `{ a b c } lmax` | `{ 1 5 3 } lmax` → 5 |
| `lmin` | 最小値 | `{ a b c } lmin` | `{ 1 5 3 } lmin` → 1 |
| `range` | 範囲（最大-最小） | `{ a b c } range` | `{ 1 5 3 } range` → 4 |
| `count` | 要素数 | `{ a b c } count` | `{ 1 2 3 4 5 } count` → 5 |

**注**: `lmax`/`lmin` は二項関数の `max`/`min` と区別するため、`l`（list）を接頭辞として付けています。

### 定数

| 定数 | Unicode | 値 | 説明 |
|------|---------|-----|------|
| `pi`, `PI` | - | 3.14159... | 円周率 |
| `π` | U+03C0 | 3.14159... | 円周率 |
| `e`, `E` | - | 2.71828... | 自然対数の底 |
| `τ` | U+03C4 | 6.28318... | タウ（2π） |

## 演算子の優先順位と結合性

### 優先順位

優先順位が高い演算子は先に計算されます。

```text
優先順位 3: ^（べき乗）
優先順位 2: *, /, %, ×, ÷, ·（乗除・剰余）
優先順位 1: +, -（加減）
```

### 結合性

同じ優先順位の演算子が連続する場合の計算順序を決定します。

- **左結合**: 左から右へ計算（例: `1 - 2 - 3` = `(1 - 2) - 3`）
- **右結合**: 右から左へ計算（例: `2 ^ 3 ^ 2` = `2 ^ (3 ^ 2)`）

```text
左結合の例: 10 - 3 - 2
  中置記法: (10 - 3) - 2 = 5
  RPN: 10 3 - 2 -

右結合の例: 2 ^ 3 ^ 2
  中置記法: 2 ^ (3 ^ 2) = 512
  RPN: 2 3 2 ^ ^
```

## アルゴリズム詳解

### 1. トークン化（字句解析）

数式文字列を意味のある単位（トークン）に分割します。

```text
入力: "sqrt(16) + 2 * 3"

トークン化結果:
  [UnaryFunction: "sqrt"]
  [LeftParen: "("]
  [Number: "16"]
  [RightParen: ")"]
  [Operator: "+"]
  [Number: "2"]
  [Operator: "*"]
  [Number: "3"]
```

#### トークン化のルール

1. **空白**: スキップ
2. **数字・小数点**: 連続する数字と小数点を1つの数値トークンとして収集
3. **括弧**: `(` は LeftParen、`)` は RightParen
4. **カンマ**: `,` は Comma トークンとして認識（二項関数の引数区切り）
5. **単項マイナス**: `-` が先頭、演算子の後、左括弧の後、またはカンマの後にあり、次が数字なら負の数として処理
6. **ASCII演算子**: `+`, `-`, `*`, `/`, `%`, `^` を Operator として認識
7. **Unicode文字**: マルチバイト文字を1文字ずつ抽出し、演算子・関数・定数テーブルを参照
8. **アルファベット**: 連続する文字を関数名・定数名として収集し、単項関数・二項関数・定数のテーブルを参照

#### 単項マイナスの判定

`-` が以下の条件を満たす場合、二項演算子ではなく負の数の符号として扱います：

- 式の先頭にある場合
- 演算子の直後にある場合
- 左括弧の直後にある場合
- カンマの直後にある場合（例: `max(-3, 5)`）

```text
例: abs(-9)
  "-9" は1つの数値トークン [Number: "-9"] として認識される

例: 5 - 3
  "-" は二項演算子 [Operator: "-"] として認識される

例: 5 * -3
  "-3" は数値トークン [Number: "-3"] として認識される
```

### 2. Shunting-yard アルゴリズム（中置記法 → RPN変換）

Edsger Dijkstra が考案したアルゴリズムで、鉄道の操車場（shunting yard）の動きに似ていることから名付けられました。

#### 基本概念

- **出力キュー**: RPN形式の結果を格納
- **演算子スタック**: 演算子と関数を一時的に保持

#### アルゴリズムの手順

```text
各トークンについて:

1. 数値の場合:
   → 出力キューに追加

2. 単項関数・二項関数の場合:
   → 演算子スタックにプッシュ

3. 演算子の場合:
   → スタックトップの演算子が以下の条件を満たす間、出力キューにポップ:
     - 関数である、または
     - 優先順位が高い、または
     - 優先順位が同じで左結合である
   → 現在の演算子をスタックにプッシュ

4. 左括弧 '(' の場合:
   → 演算子スタックにプッシュ

5. カンマ ',' の場合:
   → 左括弧が見つかるまでスタックから出力キューにポップ
   （二項関数の第1引数を処理し終えるため）

6. 右括弧 ')' の場合:
   → 左括弧が見つかるまでスタックから出力キューにポップ
   → 左括弧を破棄
   → スタックトップが関数（単項または二項）なら出力キューにポップ

7. 全トークン処理後:
   → スタックに残った演算子を全て出力キューにポップ
```

#### 具体例 1: 基本的な式

```text
入力: 1 + 2 * 3

ステップ | トークン | 出力キュー | 演算子スタック | 説明
---------|----------|------------|----------------|------
1        | 1        | 1          | []             | 数値は出力へ
2        | +        | 1          | [+]            | 演算子をスタックへ
3        | 2        | 1 2        | [+]            | 数値は出力へ
4        | *        | 1 2        | [+, *]         | * は + より優先順位が高いのでスタックへ
5        | 3        | 1 2 3      | [+, *]         | 数値は出力へ
6        | 終了     | 1 2 3 * +  | []             | スタックを全て出力へ

結果: "1 2 3 * +"
```

#### 具体例 2: 括弧を含む式

```text
入力: (1 + 2) * 3

ステップ | トークン | 出力キュー | 演算子スタック | 説明
---------|----------|------------|----------------|------
1        | (        |            | [(]            | 左括弧をスタックへ
2        | 1        | 1          | [(]            | 数値は出力へ
3        | +        | 1          | [(, +]         | 演算子をスタックへ
4        | 2        | 1 2        | [(, +]         | 数値は出力へ
5        | )        | 1 2 +      | []             | ( まで出力、( を破棄
6        | *        | 1 2 +      | [*]            | 演算子をスタックへ
7        | 3        | 1 2 + 3    | [*]            | 数値は出力へ
8        | 終了     | 1 2 + 3 *  | []             | スタックを全て出力へ

結果: "1 2 + 3 *"
```

#### 具体例 3: 関数を含む式

```text
入力: sqrt(16) + 2

ステップ | トークン | 出力キュー   | 演算子スタック | 説明
---------|----------|--------------|----------------|------
1        | sqrt     |              | [sqrt]         | 関数をスタックへ
2        | (        |              | [sqrt, (]      | 左括弧をスタックへ
3        | 16       | 16           | [sqrt, (]      | 数値は出力へ
4        | )        | 16 sqrt      | []             | ( まで出力、( を破棄、関数を出力
5        | +        | 16 sqrt      | [+]            | 演算子をスタックへ
6        | 2        | 16 sqrt 2    | [+]            | 数値は出力へ
7        | 終了     | 16 sqrt 2 +  | []             | スタックを全て出力へ

結果: "16 sqrt 2 +"
```

#### 具体例 4: 右結合演算子（べき乗）

```text
入力: 2 ^ 3 ^ 2  （数学的には 2^(3^2) = 2^9 = 512）

ステップ | トークン | 出力キュー | 演算子スタック | 説明
---------|----------|------------|----------------|------
1        | 2        | 2          | []             | 数値は出力へ
2        | ^        | 2          | [^]            | 演算子をスタックへ
3        | 3        | 2 3        | [^]            | 数値は出力へ
4        | ^        | 2 3        | [^, ^]         | 右結合なので同優先順位でもスタックへ
5        | 2        | 2 3 2      | [^, ^]         | 数値は出力へ
6        | 終了     | 2 3 2 ^ ^  | []             | スタックを全て出力へ

結果: "2 3 2 ^ ^"
（右から計算: 3^2=9, 2^9=512）
```

#### 具体例 5: 二項関数

```text
入力: pow(2, 10)

ステップ | トークン | 出力キュー | 演算子スタック | 説明
---------|----------|------------|----------------|------
1        | pow      |            | [pow]          | 二項関数をスタックへ
2        | (        |            | [pow, (]       | 左括弧をスタックへ
3        | 2        | 2          | [pow, (]       | 数値は出力へ
4        | ,        | 2          | [pow, (]       | ( までポップ（何もなし）
5        | 10       | 2 10       | [pow, (]       | 数値は出力へ
6        | )        | 2 10 pow   | []             | ( まで出力、( を破棄、関数を出力

結果: "2 10 pow"
```

#### 具体例 6: ネストした二項関数

```text
入力: max(pow(2, 3), 5)

ステップ | トークン | 出力キュー     | 演算子スタック   | 説明
---------|----------|----------------|------------------|------
1        | max      |                | [max]            | 二項関数をスタックへ
2        | (        |                | [max, (]         | 左括弧をスタックへ
3        | pow      |                | [max, (, pow]    | 二項関数をスタックへ
4        | (        |                | [max, (, pow, (] | 左括弧をスタックへ
5        | 2        | 2              | [max, (, pow, (] | 数値は出力へ
6        | ,        | 2              | [max, (, pow, (] | ( までポップ（何もなし）
7        | 3        | 2 3            | [max, (, pow, (] | 数値は出力へ
8        | )        | 2 3 pow        | [max, (]         | ( まで出力、関数を出力
9        | ,        | 2 3 pow        | [max, (]         | ( までポップ（何もなし）
10       | 5        | 2 3 pow 5      | [max, (]         | 数値は出力へ
11       | )        | 2 3 pow 5 max  | []               | ( まで出力、関数を出力

結果: "2 3 pow 5 max"
（計算: pow(2,3)=8, max(8,5)=8）
```

### 3. RPN計算アルゴリズム

スタックを使用して左から右へトークンを処理します。

#### アルゴリズムの手順

```text
各トークンについて:

1. 数値の場合:
   → スタックにプッシュ

2. 二項演算子の場合:
   → スタックから2つの値をポップ（b, a の順）
   → a 演算子 b を計算
   → 結果をスタックにプッシュ

3. 単項関数の場合:
   → スタックから1つの値をポップ
   → 関数を適用
   → 結果をスタックにプッシュ

4. 二項関数の場合:
   → スタックから2つの値をポップ（b, a の順）
   → 関数(a, b) を計算
   → 結果をスタックにプッシュ

5. 全トークン処理後:
   → スタックに残った1つの値が最終結果
```

#### 具体例: RPN計算

```text
入力: 1 2 + 3 *

ステップ | トークン | スタック    | 計算
---------|----------|-------------|------
1        | 1        | [1]         |
2        | 2        | [1, 2]      |
3        | +        | [3]         | 1 + 2 = 3
4        | 3        | [3, 3]      |
5        | *        | [9]         | 3 * 3 = 9

結果: 9
```

```text
入力: 16 sqrt 2 +

ステップ | トークン | スタック    | 計算
---------|----------|-------------|------
1        | 16       | [16]        |
2        | sqrt     | [4]         | sqrt(16) = 4
3        | 2        | [4, 2]      |
4        | +        | [6]         | 4 + 2 = 6

結果: 6
```

#### リスト関数（統計関数）の計算例

```text
入力: { 2 4 6 8 } mean

ステップ | トークン | スタック              | 説明
---------|----------|----------------------|------
1        | {        | [MARKER]             | リストマーカーをプッシュ
2        | 2        | [MARKER, 2]          | 数値をプッシュ
3        | 4        | [MARKER, 2, 4]       | 数値をプッシュ
4        | 6        | [MARKER, 2, 4, 6]    | 数値をプッシュ
5        | 8        | [MARKER, 2, 4, 6, 8] | 数値をプッシュ
6        | }        | [MARKER, 2, 4, 6, 8] | 何もしない（関数で処理）
7        | mean     | [5]                  | MARKERまでの要素[2,4,6,8]を収集し平均を計算

結果: 5
```

```text
入力: { 2 4 6 8 } mean { 2 4 6 8 } stddev +

ステップ | トークン | スタック                    | 計算
---------|----------|----------------------------|------
1-7      | ...      | [5]                        | mean計算完了
8        | {        | [5, MARKER]                | 2つ目のリスト開始
9-13     | 2,4,6,8  | [5, MARKER, 2, 4, 6, 8]   | 数値をプッシュ
14       | }        | [5, MARKER, 2, 4, 6, 8]   | 何もしない
15       | stddev   | [5, 2.236]                 | 標準偏差を計算
16       | +        | [7.236]                    | 5 + 2.236

結果: 7.236（平均 + 1σ）
```

### 4. RPN → 中置記法変換アルゴリズム

RPN計算と同様にスタックを使用しますが、数値ではなく文字列（式）を積みます。

#### アルゴリズムの手順

```text
各トークンについて:

1. 数値または定数の場合:
   → そのままスタックにプッシュ

2. 二項演算子の場合:
   → スタックから2つの式をポップ（b, a の順）
   → "(a 演算子 b)" を作成してプッシュ

3. 単項関数の場合:
   → スタックから1つの式をポップ
   → "関数(式)" を作成してプッシュ

4. 二項関数の場合:
   → スタックから2つの式をポップ（b, a の順）
   → "関数(a, b)" を作成してプッシュ

5. 全トークン処理後:
   → スタックに残った式が結果
```

#### 具体例: RPN → 中置記法

```text
入力: 1 2 + 3 4 + *

ステップ | トークン | スタック
---------|----------|----------------------------------
1        | 1        | ["1"]
2        | 2        | ["1", "2"]
3        | +        | ["(1 + 2)"]
4        | 3        | ["(1 + 2)", "3"]
5        | 4        | ["(1 + 2)", "3", "4"]
6        | +        | ["(1 + 2)", "(3 + 4)"]
7        | *        | ["((1 + 2) * (3 + 4))"]

結果: "((1 + 2) * (3 + 4))"
```

```text
入力: 16 sqrt 2 +

ステップ | トークン | スタック
---------|----------|----------------------------------
1        | 16       | ["16"]
2        | sqrt     | ["sqrt(16)"]
3        | 2        | ["sqrt(16)", "2"]
4        | +        | ["(sqrt(16) + 2)"]

結果: "(sqrt(16) + 2)"
```

## コード構成

### データ構造

```cpp
// トークンの種類
enum class TokenType {
    Number,         // 数値
    Operator,       // 演算子
    UnaryFunction,  // 単項関数
    BinaryFunction, // 二項関数
    Constant,       // 定数
    LeftParen,      // 左括弧
    RightParen,     // 右括弧
    Comma           // カンマ（引数区切り）
};

// トークン
struct Token {
    TokenType type;
    std::string value;
};

// 演算子の定義（優先順位・結合性・計算関数を一元管理）
struct OperatorInfo {
    int precedence;
    bool rightAssociative;
    std::function<double(double, double)> func;
};

// 単項関数の定義
struct UnaryFunctionInfo {
    std::function<double(double)> func;
};

// 二項関数の定義
struct BinaryFunctionInfo {
    std::function<double(double, double)> func;
};

// リスト関数の定義（統計関数など）
struct ListFunctionInfo {
    std::function<double(const std::vector<double>&)> func;
};
```

### テーブル駆動設計

演算子・関数・定数は `std::unordered_map` で一元管理しています。
これにより、新しい演算子や関数の追加が容易になっています。

```cpp
// 演算子テーブル（ASCII + Unicode）
const std::unordered_map<std::string, OperatorInfo> OPERATORS = {
    {"+", {1, false, [](double a, double b) { return a + b; }}},
    {"×", {2, false, [](double a, double b) { return a * b; }}},
    // ...
};

// 単項関数テーブル
const std::unordered_map<std::string, UnaryFunctionInfo> UNARY_FUNCTIONS = {
    {"sqrt", {[](double a) { return std::sqrt(a); }}},
    {"√",    {[](double a) { return std::sqrt(a); }}},
    // ...
};

// 二項関数テーブル
const std::unordered_map<std::string, BinaryFunctionInfo> BINARY_FUNCTIONS = {
    {"pow",   {[](double a, double b) { return std::pow(a, b); }}},
    {"max",   {[](double a, double b) { return std::max(a, b); }}},
    {"min",   {[](double a, double b) { return std::min(a, b); }}},
    {"atan2", {[](double a, double b) { return std::atan2(a, b); }}},
    {"mod",   {[](double a, double b) { return std::fmod(a, b); }}},
};

// リスト関数テーブル（統計関数 - HP電卓方式）
const std::unordered_map<std::string, ListFunctionInfo> LIST_FUNCTIONS = {
    {"sum",     {[](const std::vector<double>& v) { /* 合計 */ }}},
    {"mean",    {[](const std::vector<double>& v) { /* 平均 */ }}},
    {"median",  {[](const std::vector<double>& v) { /* 中央値 */ }}},
    {"stddev",  {[](const std::vector<double>& v) { /* 母標準偏差 */ }}},
    {"var",     {[](const std::vector<double>& v) { /* 母分散 */ }}},
    // ...
};

// 定数テーブル
const std::unordered_map<std::string, double> CONSTANTS = {
    {"pi", M_PI},
    {"π",  M_PI},
    // ...
};
```

### 主要な関数

| 関数 | 説明 |
|------|------|
| `tokenize(expression)` | 数式文字列をトークン列に分割（UTF-8対応） |
| `infixToRPN(expression)` | 中置記法をRPNに変換 |
| `rpnToInfix(expression)` | RPNを中置記法に変換 |
| `calculateRPN(expression)` | RPN式を計算 |
| `getPrecedence(op)` | 演算子の優先順位を返す |
| `isOperator(s)` | 演算子かどうかを判定 |
| `isUnaryFunction(s)` | 単項関数かどうかを判定 |
| `isBinaryFunction(s)` | 二項関数かどうかを判定 |
| `isListFunction(s)` | リスト関数かどうかを判定 |
| `isConstant(s)` | 定数かどうかを判定 |
| `isRightAssociative(op)` | 右結合演算子かどうかを判定 |

### UTF-8ユーティリティ関数

| 関数 | 説明 |
|------|------|
| `utf8CharLength(c)` | 先頭バイトからUTF-8文字のバイト数を取得 |
| `utf8ExtractChar(str, pos)` | 文字列から1文字（コードポイント）を抽出 |
| `isAsciiSpace(ch)` | ASCII空白かどうかを判定 |
| `isAsciiDigit(ch)` | ASCII数字かどうかを判定 |
| `isAsciiAlpha(ch)` | ASCIIアルファベットかどうかを判定 |
| `isMultibyte(ch)` | マルチバイト文字かどうかを判定 |

## 拡張方法

テーブル駆動設計により、演算子・関数・定数の追加が簡単になりました。

### 新しい演算子を追加

`OPERATORS` テーブルに1行追加するだけです。

```cpp
// 例: モジュロ演算子 mod を追加
{"mod", {2, false, [](double a, double b) { return std::fmod(a, b); }}}
```

### 新しい単項関数を追加

`UNARY_FUNCTIONS` テーブルに1行追加するだけです。

```cpp
// 例: asin を追加
{"asin", {[](double a) { return std::asin(a); }}}

// Unicode記号も同様
{"∛", {[](double a) { return std::cbrt(a); }}}  // 立方根
```

### 新しい二項関数を追加

`BINARY_FUNCTIONS` テーブルに1行追加するだけです。

```cpp
// 例: 最大公約数を追加
{"gcd", {[](double a, double b) { return std::gcd((int)a, (int)b); }}}

// 例: 対数（底を指定）を追加
{"log_base", {[](double a, double b) { return std::log(a) / std::log(b); }}}
```

使用例:
- `gcd(12, 8)` → RPN: `12 8 gcd` → 結果: `4`
- `log_base(8, 2)` → RPN: `8 2 log_base` → 結果: `3`（log₂8 = 3）

### 新しいリスト関数（統計関数）を追加

`LIST_FUNCTIONS` テーブルに1行追加するだけです。

```cpp
// 例: 幾何平均を追加
{"gmean", {[](const std::vector<double>& v) {
    if (v.empty()) return 0.0;
    double product = 1.0;
    for (double x : v) product *= x;
    return std::pow(product, 1.0 / v.size());
}}}

// 例: 調和平均を追加
{"hmean", {[](const std::vector<double>& v) {
    if (v.empty()) return 0.0;
    double sum = 0.0;
    for (double x : v) sum += 1.0 / x;
    return v.size() / sum;
}}}
```

使用例:
- `{ 2 8 } gmean` → 結果: `4`（√(2×8) = 4）
- `{ 2 3 6 } hmean` → 結果: `3`（3 / (1/2 + 1/3 + 1/6) = 3）

### 新しい定数を追加

`CONSTANTS` テーブルに1行追加するだけです。

```cpp
// 例: 黄金比を追加
{"phi", 1.6180339887},
{"φ",   1.6180339887}   // U+03C6
```
