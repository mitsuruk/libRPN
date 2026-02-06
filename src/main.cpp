#include <iostream>
#include "librpn.hpp"

int main() {
    std::cout << "=== 通常の数式からRPNへの変換 ===" << std::endl;

    // 基本的な四則演算
    std::string infix1 = "(1 + 2) * (3 + 4)";
    std::cout << "Infix: " << infix1 << std::endl;
    std::string rpn1 = librpn::infixToRPN(infix1);
    std::cout << "RPN: " << rpn1 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn1) << std::endl;
    std::cout << std::endl;

    // 関数を使った例
    std::string infix2 = "sqrt(16) + 2";
    std::cout << "Infix: " << infix2 << std::endl;
    std::string rpn2 = librpn::infixToRPN(infix2);
    std::cout << "RPN: " << rpn2 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn2) << std::endl;
    std::cout << std::endl;

    // べき乗
    std::string infix3 = "2 ^ 3 ^ 2";  // 右結合: 2^(3^2) = 2^9 = 512
    std::cout << "Infix: " << infix3 << std::endl;
    std::string rpn3 = librpn::infixToRPN(infix3);
    std::cout << "RPN: " << rpn3 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn3) << std::endl;
    std::cout << std::endl;

    // 複合例
    std::string infix4 = "sqrt(abs(-9)) * cos(0) + log(exp(1))";
    std::cout << "Infix: " << infix4 << std::endl;
    std::string rpn4 = librpn::infixToRPN(infix4);
    std::cout << "RPN: " << rpn4 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn4) << std::endl;
    std::cout << std::endl;

    // Unicode演算子の例
    std::cout << "=== Unicode演算子・記号の例 ===" << std::endl;

    std::string infix5 = "3 × 4 ÷ 2";  // 3 * 4 / 2 = 6
    std::cout << "Infix: " << infix5 << std::endl;
    std::string rpn5 = librpn::infixToRPN(infix5);
    std::cout << "RPN: " << rpn5 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn5) << std::endl;
    std::cout << std::endl;

    std::string infix6 = "√(16) + π";  // sqrt(16) + π = 4 + 3.14159... ≈ 7.14159
    std::cout << "Infix: " << infix6 << std::endl;
    std::string rpn6 = librpn::infixToRPN(infix6);
    std::cout << "RPN: " << rpn6 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn6) << std::endl;
    std::cout << std::endl;

    std::string infix7 = "2 × π × 3";  // 円周 = 2πr (r=3)
    std::cout << "Infix: " << infix7 << " (円周 2πr, r=3)" << std::endl;
    std::string rpn7 = librpn::infixToRPN(infix7);
    std::cout << "RPN: " << rpn7 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn7) << std::endl;
    std::cout << std::endl;

    // RPN → Infix変換の例
    std::cout << "=== RPNから中置記法への変換 ===" << std::endl;

    std::string rpn8 = "1 2 + 3 4 + *";
    std::cout << "RPN: " << rpn8 << std::endl;
    std::cout << "Infix: " << librpn::rpnToInfix(rpn8) << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn8) << std::endl;
    std::cout << std::endl;

    std::string rpn9 = "16 sqrt 2 +";
    std::cout << "RPN: " << rpn9 << std::endl;
    std::cout << "Infix: " << librpn::rpnToInfix(rpn9) << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn9) << std::endl;
    std::cout << std::endl;

    std::string rpn10 = "4 3 2 ^ ^";
    std::cout << "RPN: " << rpn10 << std::endl;
    std::cout << "Infix: " << librpn::rpnToInfix(rpn10) << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn10) << std::endl;
    std::cout << std::endl;

    std::string rpn11 = "3 4 × 2 ÷";
    std::cout << "RPN: " << rpn11 << std::endl;
    std::cout << "Infix: " << librpn::rpnToInfix(rpn11) << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn11) << std::endl;
    std::cout << std::endl;

    // 二項関数の例
    std::cout << "=== 二項関数の例 ===" << std::endl;

    std::string infix12 = "pow(2, 10)";  // 2^10 = 1024
    std::cout << "Infix: " << infix12 << std::endl;
    std::string rpn12 = librpn::infixToRPN(infix12);
    std::cout << "RPN: " << rpn12 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn12) << std::endl;
    std::cout << "Back to Infix: " << librpn::rpnToInfix(rpn12) << std::endl;
    std::cout << std::endl;

    std::string infix13 = "max(3, 7) + min(10, 5)";  // 7 + 5 = 12
    std::cout << "Infix: " << infix13 << std::endl;
    std::string rpn13 = librpn::infixToRPN(infix13);
    std::cout << "RPN: " << rpn13 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn13) << std::endl;
    std::cout << "Back to Infix: " << librpn::rpnToInfix(rpn13) << std::endl;
    std::cout << std::endl;

    std::string infix14 = "atan2(1, 1)";  // π/4 ≈ 0.785398
    std::cout << "Infix: " << infix14 << std::endl;
    std::string rpn14 = librpn::infixToRPN(infix14);
    std::cout << "RPN: " << rpn14 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn14) << std::endl;
    std::cout << "Back to Infix: " << librpn::rpnToInfix(rpn14) << std::endl;
    std::cout << std::endl;

    std::string infix15 = "mod(17, 5)";  // 17 % 5 = 2
    std::cout << "Infix: " << infix15 << std::endl;
    std::string rpn15 = librpn::infixToRPN(infix15);
    std::cout << "RPN: " << rpn15 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn15) << std::endl;
    std::cout << "Back to Infix: " << librpn::rpnToInfix(rpn15) << std::endl;
    std::cout << std::endl;

    // 二項関数と単項関数の組み合わせ
    std::string infix16 = "pow(sqrt(16), 3)";  // (√16)^3 = 4^3 = 64
    std::cout << "Infix: " << infix16 << std::endl;
    std::string rpn16 = librpn::infixToRPN(infix16);
    std::cout << "RPN: " << rpn16 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn16) << std::endl;
    std::cout << "Back to Infix: " << librpn::rpnToInfix(rpn16) << std::endl;
    std::cout << std::endl;

    // ネストした二項関数
    std::string infix17 = "max(pow(2, 3), min(10, 5))";  // max(8, 5) = 8
    std::cout << "Infix: " << infix17 << std::endl;
    std::string rpn17 = librpn::infixToRPN(infix17);
    std::cout << "RPN: " << rpn17 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn17) << std::endl;
    std::cout << "Back to Infix: " << librpn::rpnToInfix(rpn17) << std::endl;
    std::cout << std::endl;

    // 統計関数の例（HP電卓方式）
    std::cout << "=== 統計関数の例（HP電卓方式） ===" << std::endl;

    // 合計
    std::string rpn18 = "{ 1 2 3 4 5 } sum";
    std::cout << "RPN: " << rpn18 << std::endl;
    std::cout << "Result (sum): " << librpn::calculateRPN(rpn18) << std::endl;
    std::cout << std::endl;

    // 平均
    std::string rpn19 = "{ 2 4 6 8 10 } mean";
    std::cout << "RPN: " << rpn19 << std::endl;
    std::cout << "Result (mean): " << librpn::calculateRPN(rpn19) << std::endl;
    std::cout << std::endl;

    // 中央値
    std::string rpn20 = "{ 3 1 4 1 5 9 2 6 } median";
    std::cout << "RPN: " << rpn20 << std::endl;
    std::cout << "Result (median): " << librpn::calculateRPN(rpn20) << std::endl;
    std::cout << std::endl;

    // 母標準偏差
    std::string rpn21 = "{ 2 4 6 8 } stddev";
    std::cout << "RPN: " << rpn21 << std::endl;
    std::cout << "Result (stddev): " << librpn::calculateRPN(rpn21) << std::endl;
    std::cout << std::endl;

    // 標本標準偏差
    std::string rpn22 = "{ 2 4 6 8 } sstddev";
    std::cout << "RPN: " << rpn22 << std::endl;
    std::cout << "Result (sstddev): " << librpn::calculateRPN(rpn22) << std::endl;
    std::cout << std::endl;

    // 母分散
    std::string rpn23 = "{ 2 4 6 8 } var";
    std::cout << "RPN: " << rpn23 << std::endl;
    std::cout << "Result (var): " << librpn::calculateRPN(rpn23) << std::endl;
    std::cout << std::endl;

    // 最大・最小
    std::string rpn24 = "{ 3 1 4 1 5 9 2 6 } lmax";
    std::cout << "RPN: " << rpn24 << std::endl;
    std::cout << "Result (lmax): " << librpn::calculateRPN(rpn24) << std::endl;

    std::string rpn25 = "{ 3 1 4 1 5 9 2 6 } lmin";
    std::cout << "RPN: " << rpn25 << std::endl;
    std::cout << "Result (lmin): " << librpn::calculateRPN(rpn25) << std::endl;
    std::cout << std::endl;

    // 範囲
    std::string rpn26 = "{ 3 1 4 1 5 9 2 6 } range";
    std::cout << "RPN: " << rpn26 << std::endl;
    std::cout << "Result (range): " << librpn::calculateRPN(rpn26) << std::endl;
    std::cout << std::endl;

    // 要素数
    std::string rpn27 = "{ 1 2 3 4 5 6 7 8 9 10 } count";
    std::cout << "RPN: " << rpn27 << std::endl;
    std::cout << "Result (count): " << librpn::calculateRPN(rpn27) << std::endl;
    std::cout << std::endl;

    // 積
    std::string rpn28 = "{ 1 2 3 4 5 } product";
    std::cout << "RPN: " << rpn28 << std::endl;
    std::cout << "Result (product): " << librpn::calculateRPN(rpn28) << std::endl;
    std::cout << std::endl;

    // 統計関数と他の演算の組み合わせ
    std::cout << "=== 統計関数と演算の組み合わせ ===" << std::endl;

    // 平均 + 標準偏差（平均から1σの上限値）
    std::string rpn29 = "{ 2 4 6 8 } mean { 2 4 6 8 } stddev +";
    std::cout << "RPN: " << rpn29 << std::endl;
    std::cout << "Result (mean + stddev): " << librpn::calculateRPN(rpn29) << std::endl;
    std::cout << std::endl;

    // 中置記法からの変換例
    std::cout << "=== 中置記法からリスト統計関数への変換 ===" << std::endl;

    std::string infix20 = "{ 10, 20, 30, 40, 50 } mean";
    std::cout << "Infix: " << infix20 << std::endl;
    std::string rpn30 = librpn::infixToRPN(infix20);
    std::cout << "RPN: " << rpn30 << std::endl;
    std::cout << "Result: " << librpn::calculateRPN(rpn30) << std::endl;

    return 0;
}
