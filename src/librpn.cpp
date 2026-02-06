#include "librpn.hpp"
#include <stack>
#include <cctype>
#include <cmath>
#include <unordered_set>
#include <algorithm>

namespace librpn {

//==============================================================================
// グローバルテーブルの定義
//==============================================================================

// ASCII演算子の文字セット（トークナイザ用）
static const std::unordered_set<char> ASCII_OPERATOR_CHARS = {'+', '-', '*', '/', '%', '^'};

// 演算子テーブル（ASCII + Unicode）
const std::unordered_map<std::string, OperatorInfo> OPERATORS = {
    // ASCII演算子
    {"+", {1, false, [](double a, double b) { return a + b; }}},
    {"-", {1, false, [](double a, double b) { return a - b; }}},
    {"*", {2, false, [](double a, double b) { return a * b; }}},
    {"/", {2, false, [](double a, double b) { return a / b; }}},
    {"%", {2, false, [](double a, double b) { return std::fmod(a, b); }}},
    {"^", {3, true,  [](double a, double b) { return std::pow(a, b); }}},
    // Unicode演算子
    {"×", {2, false, [](double a, double b) { return a * b; }}},      // U+00D7
    {"÷", {2, false, [](double a, double b) { return a / b; }}},      // U+00F7
    {"·", {2, false, [](double a, double b) { return a * b; }}},      // U+00B7 (middle dot)
};

// 単項関数テーブル（ASCII + Unicode）
const std::unordered_map<std::string, UnaryFunctionInfo> UNARY_FUNCTIONS = {
    // ASCII関数
    {"sqrt",  {[](double a) { return std::sqrt(a); }}},
    {"sin",   {[](double a) { return std::sin(a); }}},
    {"cos",   {[](double a) { return std::cos(a); }}},
    {"tan",   {[](double a) { return std::tan(a); }}},
    {"log",   {[](double a) { return std::log(a); }}},
    {"ln",    {[](double a) { return std::log(a); }}},
    {"log10", {[](double a) { return std::log10(a); }}},
    {"abs",   {[](double a) { return std::abs(a); }}},
    {"exp",   {[](double a) { return std::exp(a); }}},
    {"floor", {[](double a) { return std::floor(a); }}},
    {"ceil",  {[](double a) { return std::ceil(a); }}},
    // Unicode関数（記号として使用）
    {"√", {[](double a) { return std::sqrt(a); }}},                   // U+221A
};

// 二項関数テーブル
const std::unordered_map<std::string, BinaryFunctionInfo> BINARY_FUNCTIONS = {
    {"pow",   {[](double a, double b) { return std::pow(a, b); }}},
    {"max",   {[](double a, double b) { return std::max(a, b); }}},
    {"min",   {[](double a, double b) { return std::min(a, b); }}},
    {"atan2", {[](double a, double b) { return std::atan2(a, b); }}},
    {"mod",   {[](double a, double b) { return std::fmod(a, b); }}},
};

// リスト関数テーブル（HP電卓方式）
const std::unordered_map<std::string, ListFunctionInfo> LIST_FUNCTIONS = {
    // 合計
    {"sum", {[](const std::vector<double>& v) {
        double total = 0;
        for (double x : v) total += x;
        return total;
    }}},
    {"ΣLIST", {[](const std::vector<double>& v) {
        double total = 0;
        for (double x : v) total += x;
        return total;
    }}},

    // 積
    {"product", {[](const std::vector<double>& v) {
        double total = 1;
        for (double x : v) total *= x;
        return total;
    }}},
    {"ΠLIST", {[](const std::vector<double>& v) {
        double total = 1;
        for (double x : v) total *= x;
        return total;
    }}},

    // 平均
    {"mean", {[](const std::vector<double>& v) {
        if (v.empty()) return 0.0;
        double total = 0;
        for (double x : v) total += x;
        return total / v.size();
    }}},

    // 母分散
    {"var", {[](const std::vector<double>& v) {
        if (v.empty()) return 0.0;
        double mean = 0;
        for (double x : v) mean += x;
        mean /= v.size();
        double variance = 0;
        for (double x : v) variance += (x - mean) * (x - mean);
        return variance / v.size();
    }}},

    // 標本分散
    {"svar", {[](const std::vector<double>& v) {
        if (v.size() < 2) return 0.0;
        double mean = 0;
        for (double x : v) mean += x;
        mean /= v.size();
        double variance = 0;
        for (double x : v) variance += (x - mean) * (x - mean);
        return variance / (v.size() - 1);
    }}},

    // 母標準偏差
    {"stddev", {[](const std::vector<double>& v) {
        if (v.empty()) return 0.0;
        double mean = 0;
        for (double x : v) mean += x;
        mean /= v.size();
        double variance = 0;
        for (double x : v) variance += (x - mean) * (x - mean);
        return std::sqrt(variance / v.size());
    }}},

    // 標本標準偏差
    {"sstddev", {[](const std::vector<double>& v) {
        if (v.size() < 2) return 0.0;
        double mean = 0;
        for (double x : v) mean += x;
        mean /= v.size();
        double variance = 0;
        for (double x : v) variance += (x - mean) * (x - mean);
        return std::sqrt(variance / (v.size() - 1));
    }}},

    // 中央値
    {"median", {[](const std::vector<double>& v) {
        if (v.empty()) return 0.0;
        std::vector<double> sorted = v;
        std::sort(sorted.begin(), sorted.end());
        size_t n = sorted.size();
        if (n % 2 == 0) {
            return (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
        } else {
            return sorted[n/2];
        }
    }}},

    // 最大値
    {"lmax", {[](const std::vector<double>& v) {
        if (v.empty()) return 0.0;
        return *std::max_element(v.begin(), v.end());
    }}},

    // 最小値
    {"lmin", {[](const std::vector<double>& v) {
        if (v.empty()) return 0.0;
        return *std::min_element(v.begin(), v.end());
    }}},

    // 範囲（最大 - 最小）
    {"range", {[](const std::vector<double>& v) {
        if (v.empty()) return 0.0;
        return *std::max_element(v.begin(), v.end()) - *std::min_element(v.begin(), v.end());
    }}},

    // 要素数
    {"count", {[](const std::vector<double>& v) {
        return static_cast<double>(v.size());
    }}},
};

// 定数テーブル
const std::unordered_map<std::string, double> CONSTANTS = {
    {"pi",  M_PI},
    {"PI",  M_PI},
    {"π",   M_PI},      // U+03C0
    {"e",   M_E},
    {"E",   M_E},
    {"τ",   2 * M_PI},  // U+03C4 (tau = 2π)
};

//==============================================================================
// UTF-8 ユーティリティ関数
//==============================================================================

size_t utf8CharLength(unsigned char c) {
    if ((c & 0x80) == 0x00) return 1;       // 0xxxxxxx - ASCII
    if ((c & 0xE0) == 0xC0) return 2;       // 110xxxxx
    if ((c & 0xF0) == 0xE0) return 3;       // 1110xxxx
    if ((c & 0xF8) == 0xF0) return 4;       // 11110xxx
    return 1; // 不正なバイトは1バイトとして扱う
}

std::string utf8ExtractChar(const std::string& str, size_t pos) {
    if (pos >= str.length()) return "";
    size_t len = utf8CharLength(static_cast<unsigned char>(str[pos]));
    if (pos + len > str.length()) return "";
    return str.substr(pos, len);
}

bool isAsciiSpace(const std::string& ch) {
    return ch.length() == 1 && std::isspace(static_cast<unsigned char>(ch[0]));
}

bool isAsciiDigit(const std::string& ch) {
    return ch.length() == 1 && std::isdigit(static_cast<unsigned char>(ch[0]));
}

bool isAsciiAlpha(const std::string& ch) {
    return ch.length() == 1 && std::isalpha(static_cast<unsigned char>(ch[0]));
}

bool isMultibyte(const std::string& ch) {
    return ch.length() > 1;
}

//==============================================================================
// 判定関数
//==============================================================================

int getPrecedence(const std::string& op) {
    auto it = OPERATORS.find(op);
    return (it != OPERATORS.end()) ? it->second.precedence : 0;
}

bool isOperator(const std::string& s) {
    return OPERATORS.find(s) != OPERATORS.end();
}

bool isUnaryFunction(const std::string& s) {
    return UNARY_FUNCTIONS.find(s) != UNARY_FUNCTIONS.end();
}

bool isBinaryFunction(const std::string& s) {
    return BINARY_FUNCTIONS.find(s) != BINARY_FUNCTIONS.end();
}

bool isConstant(const std::string& s) {
    return CONSTANTS.find(s) != CONSTANTS.end();
}

bool isListFunction(const std::string& s) {
    return LIST_FUNCTIONS.find(s) != LIST_FUNCTIONS.end();
}

bool isRightAssociative(const std::string& op) {
    auto it = OPERATORS.find(op);
    return (it != OPERATORS.end()) && it->second.rightAssociative;
}

// リストマーカー用の特殊値（NaNを使用）
static const double LIST_MARKER = std::nan("");

bool isListMarker(double v) {
    return std::isnan(v);
}

//==============================================================================
// トークナイザー（UTF-8対応）
//==============================================================================

std::vector<Token> tokenize(const std::string& expression) {
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < expression.length()) {
        std::string ch = utf8ExtractChar(expression, i);
        size_t charLen = ch.length();

        // 空白はスキップ
        if (isAsciiSpace(ch)) {
            i += charLen;
            continue;
        }

        // 数字または小数点の場合
        if (isAsciiDigit(ch) || ch == ".") {
            std::string number;
            while (i < expression.length()) {
                std::string c = utf8ExtractChar(expression, i);
                if (isAsciiDigit(c) || c == ".") {
                    number += c;
                    i += c.length();
                } else {
                    break;
                }
            }
            tokens.push_back({TokenType::Number, number});
            continue;
        }

        // 左括弧
        if (ch == "(") {
            tokens.push_back({TokenType::LeftParen, "("});
            i += charLen;
            continue;
        }

        // 右括弧
        if (ch == ")") {
            tokens.push_back({TokenType::RightParen, ")"});
            i += charLen;
            continue;
        }

        // カンマ（関数の引数区切り）
        if (ch == ",") {
            tokens.push_back({TokenType::Comma, ","});
            i += charLen;
            continue;
        }

        // リスト開始（HP方式）
        if (ch == "{") {
            tokens.push_back({TokenType::ListStart, "{"});
            i += charLen;
            continue;
        }

        // リスト終了（HP方式）
        if (ch == "}") {
            tokens.push_back({TokenType::ListEnd, "}"});
            i += charLen;
            continue;
        }

        // 単項マイナス（負の数）の判定
        // 前のトークンが演算子、左括弧、カンマ、リスト開始、または先頭の場合、- は負の数の符号
        if (ch == "-") {
            bool isUnaryMinus = tokens.empty() ||
                                tokens.back().type == TokenType::Operator ||
                                tokens.back().type == TokenType::LeftParen ||
                                tokens.back().type == TokenType::Comma ||
                                tokens.back().type == TokenType::ListStart;

            if (isUnaryMinus) {
                // 次の文字が数字か小数点なら負の数として処理
                size_t nextPos = i + charLen;
                if (nextPos < expression.length()) {
                    std::string nextCh = utf8ExtractChar(expression, nextPos);
                    if (isAsciiDigit(nextCh) || nextCh == ".") {
                        std::string number = "-";
                        i = nextPos;
                        while (i < expression.length()) {
                            std::string c = utf8ExtractChar(expression, i);
                            if (isAsciiDigit(c) || c == ".") {
                                number += c;
                                i += c.length();
                            } else {
                                break;
                            }
                        }
                        tokens.push_back({TokenType::Number, number});
                        continue;
                    }
                }
            }
        }

        // ASCII演算子（単一文字）
        if (charLen == 1 && ASCII_OPERATOR_CHARS.count(ch[0])) {
            tokens.push_back({TokenType::Operator, ch});
            i += charLen;
            continue;
        }

        // マルチバイト文字の処理（Unicode演算子・関数・定数）
        if (isMultibyte(ch)) {
            // 演算子として登録されているか確認
            if (isOperator(ch)) {
                tokens.push_back({TokenType::Operator, ch});
                i += charLen;
                continue;
            }
            // 関数として登録されているか確認（√など）
            if (isUnaryFunction(ch)) {
                tokens.push_back({TokenType::UnaryFunction, ch});
                i += charLen;
                continue;
            }
            // 定数として登録されているか確認（π, τなど）
            if (isConstant(ch)) {
                tokens.push_back({TokenType::Constant, ch});
                i += charLen;
                continue;
            }
            // 未知のマルチバイト文字はスキップ
            i += charLen;
            continue;
        }

        // ASCIIアルファベット（関数名・定数名）
        if (isAsciiAlpha(ch)) {
            std::string name;
            while (i < expression.length()) {
                std::string c = utf8ExtractChar(expression, i);
                // アルファベットまたは数字（関数名にlog10などを許容）
                if (isAsciiAlpha(c) || isAsciiDigit(c)) {
                    name += c;
                    i += c.length();
                } else {
                    break;
                }
            }
            // 定数として登録されているか確認
            if (isConstant(name)) {
                tokens.push_back({TokenType::Constant, name});
            }
            // 単項関数として登録されているか確認
            else if (isUnaryFunction(name)) {
                tokens.push_back({TokenType::UnaryFunction, name});
            }
            // 二項関数として登録されているか確認
            else if (isBinaryFunction(name)) {
                tokens.push_back({TokenType::BinaryFunction, name});
            }
            // リスト関数として登録されているか確認
            else if (isListFunction(name)) {
                tokens.push_back({TokenType::ListFunction, name});
            }
            continue;
        }

        // 未知の文字はスキップ
        i += charLen;
    }

    return tokens;
}

//==============================================================================
// 中置記法 → RPN変換（Shunting-yard アルゴリズム）
//==============================================================================

std::string infixToRPN(const std::string& expression) {
    std::vector<Token> tokens = tokenize(expression);
    std::string output;
    std::stack<Token> opStack;

    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::Number:
            case TokenType::Constant:
                if (!output.empty()) output += ' ';
                output += token.value;
                break;

            case TokenType::UnaryFunction:
                opStack.push(token);
                break;

            case TokenType::Operator:
                while (!opStack.empty() &&
                       opStack.top().type != TokenType::LeftParen &&
                       opStack.top().type != TokenType::ListStart &&
                       (opStack.top().type == TokenType::UnaryFunction ||
                        getPrecedence(opStack.top().value) > getPrecedence(token.value) ||
                        (getPrecedence(opStack.top().value) == getPrecedence(token.value) &&
                         !isRightAssociative(token.value)))) {
                    output += ' ';
                    output += opStack.top().value;
                    opStack.pop();
                }
                opStack.push(token);
                break;

            case TokenType::LeftParen:
                opStack.push(token);
                break;

            case TokenType::BinaryFunction:
                opStack.push(token);
                break;

            case TokenType::ListFunction:
                // リスト関数は後置で出力するためスタックにプッシュ
                opStack.push(token);
                break;

            case TokenType::ListStart:
                // リスト開始は出力に追加し、スタックにもマーカーとしてプッシュ
                if (!output.empty()) output += ' ';
                output += "{";
                opStack.push(token);
                break;

            case TokenType::ListEnd:
                // リスト終了：ListStartまでの演算子をポップ
                while (!opStack.empty() && opStack.top().type != TokenType::ListStart) {
                    output += ' ';
                    output += opStack.top().value;
                    opStack.pop();
                }
                if (!opStack.empty()) {
                    opStack.pop(); // '{' を削除
                }
                output += " }";
                break;

            case TokenType::Comma:
                // カンマは左括弧またはリスト開始までの演算子をポップ
                while (!opStack.empty() &&
                       opStack.top().type != TokenType::LeftParen &&
                       opStack.top().type != TokenType::ListStart) {
                    output += ' ';
                    output += opStack.top().value;
                    opStack.pop();
                }
                break;

            case TokenType::RightParen:
                while (!opStack.empty() && opStack.top().type != TokenType::LeftParen) {
                    output += ' ';
                    output += opStack.top().value;
                    opStack.pop();
                }
                if (!opStack.empty()) {
                    opStack.pop(); // '(' を削除
                }
                // 関数（単項または二項）があればポップ
                if (!opStack.empty() &&
                    (opStack.top().type == TokenType::UnaryFunction ||
                     opStack.top().type == TokenType::BinaryFunction)) {
                    output += ' ';
                    output += opStack.top().value;
                    opStack.pop();
                }
                break;
        }
    }

    // 残りの演算子を出力
    while (!opStack.empty()) {
        output += ' ';
        output += opStack.top().value;
        opStack.pop();
    }

    return output;
}

//==============================================================================
// RPN計算
//==============================================================================

double calculateRPN(const std::string& expression) {
    std::stack<double> s;

    // UTF-8対応のトークン分割（空白区切り）
    std::vector<std::string> tokens;
    std::string current;
    size_t i = 0;
    while (i < expression.length()) {
        std::string ch = utf8ExtractChar(expression, i);
        if (isAsciiSpace(ch)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += ch;
        }
        i += ch.length();
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }

    for (const auto& token : tokens) {
        // リスト開始（HP方式）
        if (token == "{") {
            s.push(LIST_MARKER);
            continue;
        }

        // リスト終了（HP方式）- 何もしない（リスト関数で処理）
        if (token == "}") {
            continue;
        }

        // 演算子
        auto opIt = OPERATORS.find(token);
        if (opIt != OPERATORS.end()) {
            double b = s.top(); s.pop();
            double a = s.top(); s.pop();
            s.push(opIt->second.func(a, b));
            continue;
        }

        // 単項関数
        auto funcIt = UNARY_FUNCTIONS.find(token);
        if (funcIt != UNARY_FUNCTIONS.end()) {
            double a = s.top(); s.pop();
            s.push(funcIt->second.func(a));
            continue;
        }

        // 二項関数
        auto binFuncIt = BINARY_FUNCTIONS.find(token);
        if (binFuncIt != BINARY_FUNCTIONS.end()) {
            double b = s.top(); s.pop();
            double a = s.top(); s.pop();
            s.push(binFuncIt->second.func(a, b));
            continue;
        }

        // リスト関数（統計関数など）
        auto listFuncIt = LIST_FUNCTIONS.find(token);
        if (listFuncIt != LIST_FUNCTIONS.end()) {
            // リストマーカーまでの要素を収集
            std::vector<double> values;
            while (!s.empty() && !isListMarker(s.top())) {
                values.push_back(s.top());
                s.pop();
            }
            if (!s.empty()) {
                s.pop(); // リストマーカーを削除
            }
            // 順序を反転（スタックから取り出したため逆順になっている）
            std::reverse(values.begin(), values.end());
            // リスト関数を適用
            s.push(listFuncIt->second.func(values));
            continue;
        }

        // 定数
        auto constIt = CONSTANTS.find(token);
        if (constIt != CONSTANTS.end()) {
            s.push(constIt->second);
            continue;
        }

        // 数字
        s.push(std::stod(token));
    }

    return s.top();
}

//==============================================================================
// RPN → 中置記法変換
//==============================================================================

std::string rpnToInfix(const std::string& expression) {
    std::stack<std::string> s;

    // UTF-8対応のトークン分割（空白区切り）
    std::vector<std::string> tokens;
    std::string current;
    size_t i = 0;
    while (i < expression.length()) {
        std::string ch = utf8ExtractChar(expression, i);
        if (isAsciiSpace(ch)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += ch;
        }
        i += ch.length();
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }

    for (const auto& token : tokens) {
        // 演算子
        if (isOperator(token)) {
            std::string b = s.top(); s.pop();
            std::string a = s.top(); s.pop();
            s.push("(" + a + " " + token + " " + b + ")");
            continue;
        }

        // 単項関数
        if (isUnaryFunction(token)) {
            std::string a = s.top(); s.pop();
            s.push(token + "(" + a + ")");
            continue;
        }

        // 二項関数
        if (isBinaryFunction(token)) {
            std::string b = s.top(); s.pop();
            std::string a = s.top(); s.pop();
            s.push(token + "(" + a + ", " + b + ")");
            continue;
        }

        // 数値または定数
        s.push(token);
    }

    return s.top();
}

} // namespace librpn
