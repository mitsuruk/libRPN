#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace librpn {

//==============================================================================
// トークン定義
//==============================================================================

// トークンの種類
enum class TokenType {
    Number,
    Operator,
    UnaryFunction,
    BinaryFunction,
    ListFunction,    // リストを引数に取る関数（統計関数など）
    Constant,
    LeftParen,
    RightParen,
    Comma,
    ListStart,       // { - HP方式のリスト開始
    ListEnd          // } - HP方式のリスト終了
};

// トークン構造体
struct Token {
    TokenType type;
    std::string value;
};

//==============================================================================
// 演算子・関数・定数の情報構造体
//==============================================================================

// 演算子の定義
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

//==============================================================================
// グローバルテーブル（extern宣言）
//==============================================================================

extern const std::unordered_map<std::string, OperatorInfo> OPERATORS;
extern const std::unordered_map<std::string, UnaryFunctionInfo> UNARY_FUNCTIONS;
extern const std::unordered_map<std::string, BinaryFunctionInfo> BINARY_FUNCTIONS;
extern const std::unordered_map<std::string, ListFunctionInfo> LIST_FUNCTIONS;
extern const std::unordered_map<std::string, double> CONSTANTS;

//==============================================================================
// UTF-8 ユーティリティ関数
//==============================================================================

// UTF-8の先頭バイトから文字のバイト数を取得
size_t utf8CharLength(unsigned char c);

// UTF-8文字列から1文字（コードポイント）を抽出
std::string utf8ExtractChar(const std::string& str, size_t pos);

// UTF-8文字がASCII空白かどうか
bool isAsciiSpace(const std::string& ch);

// UTF-8文字がASCII数字かどうか
bool isAsciiDigit(const std::string& ch);

// UTF-8文字がASCIIアルファベットかどうか
bool isAsciiAlpha(const std::string& ch);

// UTF-8文字がマルチバイト（非ASCII）かどうか
bool isMultibyte(const std::string& ch);

//==============================================================================
// 判定関数
//==============================================================================

// 演算子の優先順位を返す
int getPrecedence(const std::string& op);

// 演算子かどうかを判定
bool isOperator(const std::string& s);

// 単項関数かどうかを判定
bool isUnaryFunction(const std::string& s);

// 二項関数かどうかを判定
bool isBinaryFunction(const std::string& s);

// 定数かどうかを判定
bool isConstant(const std::string& s);

// リスト関数かどうかを判定
bool isListFunction(const std::string& s);

// 右結合演算子かどうか
bool isRightAssociative(const std::string& op);

// リストマーカーかどうかを判定
bool isListMarker(double v);

//==============================================================================
// 主要な変換・計算関数
//==============================================================================

// 数式文字列をトークン列に分割（UTF-8対応）
std::vector<Token> tokenize(const std::string& expression);

// 中置記法をRPNに変換
std::string infixToRPN(const std::string& expression);

// RPNを中置記法に変換
std::string rpnToInfix(const std::string& expression);

// RPN式を計算
double calculateRPN(const std::string& expression);

} // namespace librpn
