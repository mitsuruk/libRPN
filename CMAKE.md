# CMakeLists.txt 詳細解説

## 概要

このCMakeLists.txtは、macOS上でC/C++プロジェクトをビルドするための汎用テンプレートです。
CMake 3.20以降が必要で、AppleClangコンパイラに最適化されています。

---

## ファイル構造と処理フロー

```text
CMakeLists.txt の処理順序:

1. 初期設定 (1-29行目)
   └─ CMake最小バージョン、診断出力、compile_commands.json生成

2. プロジェクト定義 (31-84行目)
   └─ プロジェクト名決定、ソースファイル検索、ターゲット作成

3. コンパイラ設定 (86-194行目)
   └─ 出力ディレクトリ、言語標準、ビルドタイプ、サニタイザー

4. ビルド後処理 (224-246行目)
   └─ macOS固有の診断コマンド

5. インクルードパス設定 (248-267行目)
   └─ プロジェクト内外のヘッダディレクトリ

6. 拡張モジュール読み込み (269-441行目)
   └─ .cmakeファイルの条件付きインクルード

7. ユーティリティ関数定義 (289-482行目)
   └─ copy_files(), find_pkg_config(), link_latest_package()
```

---

## セクション別詳細解説

### 1. ヘッダとCMake最小バージョン (1-11行目)

```cmake
cmake_minimum_required(VERSION 3.20)
```

**期待される動作:**

- CMake 3.20未満では設定エラーとなり停止
- 3.20が必要な理由: `target_compile_features()`のC17/C++17サポート、ジェネレータ式の拡張機能

---

### 2. 診断情報の出力 (13-25行目)

```cmake
message(STATUS "CMake version: ${CMAKE_VERSION}")

if(APPLE OR UNIX)
    execute_process(
        COMMAND date "+%Y-%m-%d %H:%M:%S"
        OUTPUT_VARIABLE LOCAL_TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()
message(STATUS "Local time: ${LOCAL_TIME}")
```

**期待される動作:**

- CMakeのバージョンを表示
- macOS/Linux環境では現在のローカル時刻を取得・表示
- ビルドログにタイムスタンプを残すことでデバッグを容易にする

**出力例:**

```text
-- CMake version: 3.28.1
-- Local time: 2026-01-15 14:30:00
```

---

### 3. compile_commands.json生成 (28-29行目)

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

**期待される動作:**

- ビルドディレクトリに`compile_commands.json`を自動生成
- clangd、ccls等のLanguage Serverがコード補完・診断に使用
- 後段のPOST_BUILDでソースディレクトリにもコピーされる

---

### 4. プロジェクト名の決定 (31-40行目)

```cmake
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(a.out VERSION 0.0.1)
else()
    get_filename_component(DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    project(${DIR_NAME}.out VERSION 0.0.1)
endif()
```

**期待される動作:**

| 状況 | プロジェクト名 | 例 |
|------|----------------|-----|
| トップレベルプロジェクト | `a.out` | 直接`cmake ..`を実行 |
| サブディレクトリ | `<ディレクトリ名>.out` | `add_subdirectory()`で呼ばれた場合、例: `Basic.out` |

**設計意図:**

- 複数プロジェクトを`add_subdirectory()`で統合する際、名前衝突を防ぐ
- ディレクトリ名から自動的に実行ファイル名を決定

---

### 5. ディレクトリ情報の診断出力 (42-50行目)

```cmake
message(STATUS "CMAKE_SOURCE_DIR         = ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR         = ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")
```

**変数の意味:**

| 変数 | 説明 |
|------|------|
| `CMAKE_SOURCE_DIR` | 最上位のCMakeLists.txtがあるディレクトリ |
| `CMAKE_BINARY_DIR` | 最上位のビルドディレクトリ |
| `CMAKE_CURRENT_SOURCE_DIR` | 現在処理中のCMakeLists.txtがあるディレクトリ |
| `CMAKE_CURRENT_BINARY_DIR` | 現在処理中のビルドディレクトリ |

---

### 6. ソースディレクトリの決定 (53-64行目)

```cmake
if(NOT GTEST)
    if(EXISTS ${PROJECT_SOURCE_DIR}/src)
        set(LOCAL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
    else()
        set(LOCAL_SOURCE_DIR ${PROJECT_SOURCE_DIR})
    endif()
else()
    set(LOCAL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/test)
endif()
```

**期待される動作:**

| 条件 | LOCAL_SOURCE_DIR |
|------|------------------|
| 通常ビルド + `src/`存在 | `${PROJECT_SOURCE_DIR}/src` |
| 通常ビルド + `src/`なし | `${PROJECT_SOURCE_DIR}` (ルート) |
| `-DGTEST=true` | `${PROJECT_SOURCE_DIR}/test` |

**設計意図:**

- `src/`ディレクトリがあればそこからソースを検索（推奨構成）
- なければプロジェクトルートから検索（シンプルなプロジェクト用）
- GoogleTest時は`test/`ディレクトリを使用

---

### 7. ソースファイルの収集 (66-81行目)

```cmake
file(GLOB SRC
    CONFIGURE_DEPENDS
    "${LOCAL_SOURCE_DIR}/*.cpp"
    "${LOCAL_SOURCE_DIR}/*.cc"
    "${LOCAL_SOURCE_DIR}/*.cxx"
    "${LOCAL_SOURCE_DIR}/*.c"
    "${LOCAL_SOURCE_DIR}/*.m"
    "${LOCAL_SOURCE_DIR}/*.mm"
)

if(NOT SRC)
    message(WARNING "No source files found in ${LOCAL_SOURCE_DIR}")
endif()
```

**期待される動作:**

- 指定ディレクトリ内の全ソースファイルを自動収集
- `CONFIGURE_DEPENDS`: ファイル追加/削除時にCMakeを自動再実行
- ソースがない場合は警告を表示（ビルドは続行）

**対応ファイル形式:**

| 拡張子 | 言語 |
|--------|------|
| `.cpp`, `.cc`, `.cxx` | C++ |
| `.c` | C |
| `.m` | Objective-C |
| `.mm` | Objective-C++ |

---

### 8. 実行ファイルターゲットの作成 (83-84行目)

```cmake
add_executable(${PROJECT_NAME} ${SRC})
```

**期待される動作:**

- `${PROJECT_NAME}`（例: `a.out`）という名前の実行ファイルターゲットを作成
- 収集した全ソースファイルをコンパイル対象として登録

---

### 9. コンパイラ情報の診断出力 (86-94行目)

```cmake
message(STATUS "CMAKE_CXX_COMPILER_ID      = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER         = ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_C_COMPILER           = ${CMAKE_C_COMPILER}")
```

**出力例 (macOS):**

```text
-- CMAKE_CXX_COMPILER_ID      = AppleClang
-- CMAKE_CXX_COMPILER_VERSION = 16.0.0.16000026
-- CMAKE_CXX_COMPILER         = /usr/bin/clang++
-- CMAKE_C_COMPILER           = /usr/bin/clang
```

---

### 10. 出力ディレクトリの設定 (97-127行目)

```cmake
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
```

**期待される動作:**

| プロパティ | デフォルト値 | 用途 |
|------------|--------------|------|
| `RUNTIME_OUTPUT_DIRECTORY` | `${CMAKE_BINARY_DIR}` | 実行ファイル出力先 |
| `ARCHIVE_OUTPUT_DIRECTORY` | `${CMAKE_BINARY_DIR}` | 静的ライブラリ出力先 |
| `LIBRARY_OUTPUT_DIRECTORY` | `${CMAKE_BINARY_DIR}` | 共有ライブラリ出力先 |

**設計意図:**

- 親プロジェクトが出力ディレクトリを指定している場合はそれを尊重
- 指定がなければビルドディレクトリ直下に出力

---

### 11. 言語標準の設定 (129-133行目)

```cmake
target_compile_features(${PROJECT_NAME} PRIVATE c_std_17 cxx_std_17)
```

**期待される動作:**

- C17標準を要求（`-std=c17`相当）
- C++17標準を要求（`-std=c++17`相当）
- コンパイラが対応していない場合はエラー

**C++17で利用可能になる主な機能:**

- 構造化束縛 (`auto [a, b] = pair;`)
- `if constexpr`
- `std::optional`, `std::variant`, `std::string_view`
- インライン変数
- 折りたたみ式

---

### 12. プロジェクト全体のコンパイル定義 (135-144行目)

```cmake
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_NAME="${PROJECT_NAME}"
    PROJECT_VERSION="${PROJECT_VERSION}"
    ONE_=1
    TWO_=2
    THREE_=3
)
```

**期待される動作:**

全ソースファイルで以下のマクロが使用可能:

```cpp
// C++コードでの使用例
std::cout << PROJECT_NAME << std::endl;    // "a.out"
std::cout << PROJECT_VERSION << std::endl; // "0.0.1"
std::cout << ONE_ + TWO_ + THREE_ << std::endl; // 6
```

---

### 13. ファイル固有のコンパイル定義 (146-163行目)

```cmake
set_source_files_properties(
    ${LOCAL_SOURCE_DIR}/main.cpp
    PROPERTIES COMPILE_DEFINITIONS
    "MAIN_FILE_=1;MSG1=\"MSG1\";MSG2=\"Hello\""
)
```

**期待される動作:**

- `main.cpp`のみで有効なマクロを定義
- 他のソースファイルからは参照不可

```cpp
// main.cpp でのみ使用可能
#ifdef MAIN_FILE_
    std::cout << MSG1 << std::endl;  // "MSG1"
    std::cout << MSG2 << std::endl;  // "Hello"
#endif
```

**用途:**

- ファイルごとに異なる設定を適用したい場合
- メインファイルの識別

---

### 14. ビルドタイプの設定 (165-191行目)

```cmake
if(DEBUG)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:-O2 -Wall -funroll-loops>
    $<$<CONFIG:Debug>:-g -O0 -Wall>
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:NDEBUG>
)
```

**期待される動作:**

| ビルドタイプ | コマンド | コンパイルオプション | 定義マクロ |
|--------------|----------|----------------------|------------|
| Release (デフォルト) | `cmake ..` | `-O2 -Wall -funroll-loops` | `NDEBUG` |
| Debug | `cmake -DDEBUG=true ..` | `-g -O0 -Wall` | `DEBUG_BUILD` |

**オプションの意味:**

| オプション | 説明 |
|------------|------|
| `-O2` | 最適化レベル2（速度重視） |
| `-O0` | 最適化なし（デバッグ用） |
| `-g` | デバッグ情報を含める |
| `-Wall` | 一般的な警告を有効化 |
| `-funroll-loops` | ループ展開による最適化 |

---

### 15. 文字エンコーディング設定 (193-194行目)

```cmake
target_compile_options(${PROJECT_NAME} PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
```

**期待される動作:**

- ソースファイルをUTF-8として読み込み
- 実行時の文字列もUTF-8として扱う
- 日本語等のマルチバイト文字を正しく処理

---

### 16. サニタイザー設定 (196-222行目)

```cmake
if(SANI)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
        -g
        -O1
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
        -fsanitize=undefined
    )
endif()
```

**期待される動作:**

`cmake -DSANI=true ..`で有効化:

| サニタイザー | 検出対象 |
|--------------|----------|
| AddressSanitizer (ASan) | バッファオーバーフロー、use-after-free、メモリリーク |
| UndefinedBehaviorSanitizer (UBSan) | 未定義動作（整数オーバーフロー、null参照等） |

**追加オプションの意味:**

| オプション | 説明 |
|------------|------|
| `-fno-omit-frame-pointer` | スタックトレースを正確に取得 |
| `-fno-optimize-sibling-calls` | 末尾呼び出し最適化を無効化（トレース精度向上） |
| `-O1` | 最小限の最適化（ASanとの互換性） |

---

### 17. ビルド後処理 (224-246行目)

```cmake
if(APPLE)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND "date" "-R"
        COMMAND "pwd"
        COMMAND "lipo" "-archs" "${BINARY_OUTPUT_DIR}/${PROJECT_NAME}"
        COMMAND "otool" "-L" "${BINARY_OUTPUT_DIR}/${PROJECT_NAME}"
        COMMENT "${PROJECT_NAME} information"
    )

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
        COMMENT "Copying compile_commands.json to source directory"
    )
endif()
```

**期待される動作:**

ビルド成功後に自動実行:

1. 現在日時を表示
2. 作業ディレクトリを表示
3. `lipo -archs`: 実行ファイルのアーキテクチャを表示（例: `x86_64`, `arm64`）
4. `otool -L`: リンクされている動的ライブラリを一覧表示
5. `compile_commands.json`をソースディレクトリにコピー

**出力例:**

```text
Mon, 15 Jan 2026 14:30:00 +0900
/Users/user/project/build
arm64
/Users/user/project/build/a.out:
    /usr/lib/libc++.1.dylib (compatibility version 1.0.0)
    /usr/lib/libSystem.B.dylib (compatibility version 1.0.0)
```

---

### 18. インクルードパス設定 (248-267行目)

```cmake
if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/src/include)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/include)
endif()
if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/include)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
endif()

if(IS_DIRECTORY /usr/local/include)
    target_include_directories(${PROJECT_NAME} PRIVATE /usr/local/include)
endif()
if(IS_DIRECTORY $ENV{HOME}/include)
    target_include_directories(${PROJECT_NAME} PRIVATE $ENV{HOME}/include)
endif()
if(IS_DIRECTORY ../include)
    target_include_directories(${PROJECT_NAME} PRIVATE ../include)
endif()
```

**期待される動作:**

以下のディレクトリが存在すればインクルードパスに追加:

| 優先度 | パス | 用途 |
|--------|------|------|
| 1 | `${PROJECT_SOURCE_DIR}/src/include` | プロジェクト固有ヘッダ |
| 2 | `${PROJECT_SOURCE_DIR}/include` | プロジェクト固有ヘッダ（代替） |
| 3 | `/usr/local/include` | システム共通ヘッダ |
| 4 | `$HOME/include` | ユーザー固有ヘッダ |
| 5 | `../include` | 親ディレクトリの共有ヘッダ |

---

### 19. 拡張モジュールの読み込み (269-441行目)

```cmake
# macOS専用
if(APPLE)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/apple.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/apple.cmake)
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/framework.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/framework.cmake)
    endif()
endif()

# 共通
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/install.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/install.cmake)
endif()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost.cmake)
endif()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CodeGenerators.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CodeGenerators.cmake)
endif()
```

**期待される動作:**

| ファイル | 条件 | 機能 |
|----------|------|------|
| `cmake/apple.cmake` | macOS + ファイル存在 | Homebrew設定、Metal C++サポート |
| `cmake/framework.cmake` | macOS + ファイル存在 | Appleフレームワークリンク |
| `cmake/install.cmake` | ファイル存在 | インストールルール |
| `cmake/boost.cmake` | ファイル存在 | Boostライブラリ統合 |
| `cmake/CodeGenerators.cmake` | ファイル存在 | Flex/Bison/gRPC/ANTLR統合 |

---

### 20. GoogleTest統合 (392-420行目)

```cmake
if(GTEST)
    find_package(GTest REQUIRED)
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        GTest::gmock
        GTest::gmock_main
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS "GTEST")
endif()
```

**期待される動作:**

`cmake -DGTEST=true ..`で有効化:

1. GoogleTestパッケージを検索
2. GoogleMock + GoogleTestをリンク
3. `GTEST`マクロを定義

**使用例:**

```cpp
#ifdef GTEST
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(MyTest, BasicAssertion) {
    EXPECT_EQ(1 + 1, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif
```

---

## ユーティリティ関数

### copy_files() (303-330行目)

指定した拡張子のファイルをビルドディレクトリにコピーする。

```cmake
function(copy_files TARGET_NAME S_DIR FILE_EXT TAG_DIR)
```

**引数:**

| 引数 | 説明 | 例 |
|------|------|-----|
| `TARGET_NAME` | カスタムターゲット名 | `copy_json_files` |
| `S_DIR` | コピー元ディレクトリ | `${CMAKE_CURRENT_SOURCE_DIR}/json` |
| `FILE_EXT` | ファイル拡張子 | `json` |
| `TAG_DIR` | コピー先ディレクトリ | `${BINARY_OUTPUT_DIR}` |

**使用例:**

```cmake
copy_files(copy_json_files "${CMAKE_CURRENT_SOURCE_DIR}/json" "json" "${BINARY_OUTPUT_DIR}")
add_dependencies(${PROJECT_NAME} copy_json_files)
```

---

### find_pkg_config() (338-376行目)

pkg-configを使用してパッケージを検索・リンクする。

```cmake
function(find_pkg_config target [scope] package)
```

**引数:**

| 引数 | 説明 | 例 |
|------|------|-----|
| `target` | リンク先ターゲット | `${PROJECT_NAME}` |
| `scope` | リンクスコープ（省略可） | `PRIVATE`, `PUBLIC`, `INTERFACE` |
| `package` | パッケージ名 | `cairo`, `gtk+-3.0` |

**使用例:**

```cmake
find_pkg_config(${PROJECT_NAME} PRIVATE cairo)
find_pkg_config(${PROJECT_NAME} PUBLIC gtk+-3.0)
```

---

### link_latest_package() (454-481行目) - macOS専用

Homebrewでインストールされたパッケージの最新バージョンを手動リンクする。

```cmake
function(link_latest_package PACKAGE_NAME LIB_FILES)
```

**引数:**

| 引数 | 説明 | 例 |
|------|------|-----|
| `PACKAGE_NAME` | Cellarディレクトリ名 | `libomp`, `boost` |
| `LIB_FILES` | ライブラリファイル名 | `libomp.dylib` |

**使用例:**

```cmake
link_latest_package(libomp "libomp.dylib")
link_latest_package(boost "libboost_system.dylib;libboost_filesystem.dylib")
```

**期待される動作:**

1. `brew --prefix`でHomebrewのインストールパスを取得
2. `Cellar/<PACKAGE_NAME>/`内の最新バージョンディレクトリを検索
3. `include/`と`lib/`をパスに追加
4. 指定ライブラリをリンク

---

## ビルドオプション一覧

| オプション | 説明 | 使用例 |
|------------|------|--------|
| `-DDEBUG=true` | デバッグビルド | `cmake -DDEBUG=true ..` |
| `-DSANI=true` | サニタイザー有効化 | `cmake -DSANI=true ..` |
| `-DGTEST=true` | GoogleTest有効化 | `cmake -DGTEST=true ..` |

---

## 拡張モジュール詳細

### apple.cmake

- Homebrewインストールディレクトリの自動検出
- `CMAKE_PREFIX_PATH`への追加
- Metal C++ヘッダのサポート（`/usr/local/include/metal-cpp`）

### boost.cmake

- Boostライブラリの検索とリンク
- `BOOST_COMPONENTS`リストで使用コンポーネントを選択
- 主要コンポーネント: `headers`, `filesystem`, `regex`, `json`, `program_options`

### framework.cmake

- macOSシステムフレームワークのリンク設定
- 200以上のフレームワークをコメント付きでリスト化
- 使用したいフレームワークのコメントを外して有効化

### install.cmake

- `cmake --install`時のインストールルール
- 実行ファイル、ヘッダ、ドキュメントのインストール先設定

### packageInstall.cmake

- CMakeパッケージとしてエクスポート
- 他プロジェクトから`find_package()`で利用可能に

### sqlite3.cmake

- SQLite3アマルガメーション版の自動ダウンロード
- スタティックライブラリとしてビルド
- `download/sqlite3/`にキャッシュ

### CodeGenerators.cmake

- Flex/Bison: `grammar/`ディレクトリの`.y`/`.l`ファイルを処理
- gRPC/Protobuf: `protos/`ディレクトリの`.proto`ファイルを処理
- ANTLR: `antlr/`ディレクトリの`.g4`ファイルを処理

---

## トラブルシューティング

### ソースファイルが見つからない

```text
CMake Warning: No source files found in /path/to/project/src
```

**対処:** `src/`またはプロジェクトルートに`.cpp`等のソースファイルを配置

### Homebrewパッケージが見つからない

```text
FATAL_ERROR: No valid version directory in /opt/homebrew/Cellar/package
```

**対処:** `brew install <package_name>`でパッケージをインストール

### pkg-configエラー

```text
Could not find a package configuration file provided by "PkgConfig"
```

**対処:** `brew install pkg-config`でpkg-configをインストール

### サニタイザーでリンクエラー

```text
ld: library not found for -lasan
```

**対処:** AppleClang以外のコンパイラではサニタイザーが利用できない場合がある

---

## 参考情報

- **CMakeバージョン:** 3.20以上必須
- **C標準:** C17
- **C++標準:** C++17
- **対応OS:** macOS（一部機能はLinuxでも動作）
- **推奨コンパイラ:** AppleClang
- **ライセンス:** CC0 1.0 Universal（パブリックドメイン）

---

## 更新履歴

- 2025-11-26: 初版作成
- 2026-01-15: 詳細解説を追加、セクション別に処理内容を説明
