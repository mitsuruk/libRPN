# =============================================================================
# RPN Library - Google Test Configuration
# =============================================================================

# テストターゲット名
set(TEST_TARGET_NAME rpn_test)

# テストソースファイル
set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rpn_test.cpp
)

# ライブラリソースファイル（テスト対象）
set(LIB_SOURCES
    ${PROJECT_SOURCE_DIR}/src/librpn.cpp
)

# テスト実行ファイルを作成
add_executable(${TEST_TARGET_NAME} ${TEST_SOURCES} ${LIB_SOURCES})

# C++17を使用
target_compile_features(${TEST_TARGET_NAME} PRIVATE cxx_std_17)

# インクルードディレクトリ
target_include_directories(${TEST_TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Google Testをリンク
find_package(GTest REQUIRED)
target_link_libraries(${TEST_TARGET_NAME}
    PRIVATE
    GTest::gtest
    GTest::gtest_main
)

# CTestに登録
include(GoogleTest)
gtest_discover_tests(${TEST_TARGET_NAME})

# コンパイルオプション
target_compile_options(${TEST_TARGET_NAME} PRIVATE
    -Wall
    -finput-charset=UTF-8
    -fexec-charset=UTF-8
)

# 出力ディレクトリ
set_target_properties(${TEST_TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

message(STATUS "===============================================================")
message(STATUS "Test target: ${TEST_TARGET_NAME}")
message(STATUS "Test sources: ${TEST_SOURCES}")
message(STATUS "===============================================================")
