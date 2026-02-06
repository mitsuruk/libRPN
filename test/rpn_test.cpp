#include <gtest/gtest.h>
#include "../src/librpn.hpp"
#include <cmath>

//==============================================================================
// infixToRPN テスト
//==============================================================================

class InfixToRPNTest : public ::testing::Test {};

TEST_F(InfixToRPNTest, BasicArithmetic) {
    EXPECT_EQ(librpn::infixToRPN("1 + 2"), "1 2 +");
    EXPECT_EQ(librpn::infixToRPN("3 - 4"), "3 4 -");
    EXPECT_EQ(librpn::infixToRPN("5 * 6"), "5 6 *");
    EXPECT_EQ(librpn::infixToRPN("8 / 2"), "8 2 /");
}

TEST_F(InfixToRPNTest, Precedence) {
    EXPECT_EQ(librpn::infixToRPN("1 + 2 * 3"), "1 2 3 * +");
    EXPECT_EQ(librpn::infixToRPN("2 * 3 + 4"), "2 3 * 4 +");
    EXPECT_EQ(librpn::infixToRPN("10 - 3 * 2"), "10 3 2 * -");
}

TEST_F(InfixToRPNTest, Parentheses) {
    EXPECT_EQ(librpn::infixToRPN("(1 + 2) * 3"), "1 2 + 3 *");
    EXPECT_EQ(librpn::infixToRPN("(1 + 2) * (3 + 4)"), "1 2 + 3 4 + *");
    EXPECT_EQ(librpn::infixToRPN("((1 + 2))"), "1 2 +");
}

TEST_F(InfixToRPNTest, RightAssociative) {
    EXPECT_EQ(librpn::infixToRPN("2 ^ 3 ^ 2"), "2 3 2 ^ ^");
    EXPECT_EQ(librpn::infixToRPN("2 ^ 3"), "2 3 ^");
}

TEST_F(InfixToRPNTest, UnaryFunctions) {
    EXPECT_EQ(librpn::infixToRPN("sqrt(16)"), "16 sqrt");
    EXPECT_EQ(librpn::infixToRPN("abs(-5)"), "-5 abs");
    EXPECT_EQ(librpn::infixToRPN("sqrt(16) + 2"), "16 sqrt 2 +");
}

TEST_F(InfixToRPNTest, BinaryFunctions) {
    EXPECT_EQ(librpn::infixToRPN("pow(2, 10)"), "2 10 pow");
    EXPECT_EQ(librpn::infixToRPN("max(3, 7)"), "3 7 max");
    EXPECT_EQ(librpn::infixToRPN("min(10, 5)"), "10 5 min");
}

TEST_F(InfixToRPNTest, NestedFunctions) {
    EXPECT_EQ(librpn::infixToRPN("sqrt(abs(-9))"), "-9 abs sqrt");
    EXPECT_EQ(librpn::infixToRPN("pow(sqrt(16), 3)"), "16 sqrt 3 pow");
    EXPECT_EQ(librpn::infixToRPN("max(pow(2, 3), 5)"), "2 3 pow 5 max");
}

TEST_F(InfixToRPNTest, UnicodeOperators) {
    EXPECT_EQ(librpn::infixToRPN("3 × 4"), "3 4 ×");
    EXPECT_EQ(librpn::infixToRPN("8 ÷ 2"), "8 2 ÷");
    EXPECT_EQ(librpn::infixToRPN("√(16)"), "16 √");
}

TEST_F(InfixToRPNTest, Constants) {
    EXPECT_EQ(librpn::infixToRPN("pi + 1"), "pi 1 +");
    EXPECT_EQ(librpn::infixToRPN("2 * π"), "2 π *");
}

TEST_F(InfixToRPNTest, ListFunctions) {
    EXPECT_EQ(librpn::infixToRPN("{ 1, 2, 3 } sum"), "{ 1 2 3 } sum");
    EXPECT_EQ(librpn::infixToRPN("{ 2, 4, 6 } mean"), "{ 2 4 6 } mean");
}

//==============================================================================
// calculateRPN テスト
//==============================================================================

class CalculateRPNTest : public ::testing::Test {};

TEST_F(CalculateRPNTest, BasicArithmetic) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("1 2 +"), 3.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("5 3 -"), 2.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("4 5 *"), 20.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("10 2 /"), 5.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("7 3 %"), 1.0);
}

TEST_F(CalculateRPNTest, ComplexExpressions) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("1 2 + 3 *"), 9.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("1 2 + 3 4 + *"), 21.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("2 3 2 ^ ^"), 512.0);
}

TEST_F(CalculateRPNTest, UnaryFunctions) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("16 sqrt"), 4.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("-5 abs"), 5.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("0 cos"), 1.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("0 sin"), 0.0);
    EXPECT_NEAR(librpn::calculateRPN("1 exp"), M_E, 1e-10);
    EXPECT_NEAR(librpn::calculateRPN("1 exp log"), 1.0, 1e-10);
}

TEST_F(CalculateRPNTest, BinaryFunctions) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("2 10 pow"), 1024.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("3 7 max"), 7.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("10 5 min"), 5.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("17 5 mod"), 2.0);
    EXPECT_NEAR(librpn::calculateRPN("1 1 atan2"), M_PI / 4, 1e-10);
}

TEST_F(CalculateRPNTest, Constants) {
    EXPECT_NEAR(librpn::calculateRPN("pi"), M_PI, 1e-10);
    EXPECT_NEAR(librpn::calculateRPN("e"), M_E, 1e-10);
    EXPECT_NEAR(librpn::calculateRPN("π"), M_PI, 1e-10);
    EXPECT_NEAR(librpn::calculateRPN("τ"), 2 * M_PI, 1e-10);
}

TEST_F(CalculateRPNTest, UnicodeOperators) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("3 4 ×"), 12.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("8 2 ÷"), 4.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("16 √"), 4.0);
}

TEST_F(CalculateRPNTest, NegativeNumbers) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("-5 3 +"), -2.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("-3 -2 *"), 6.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("-9 abs"), 9.0);
}

//==============================================================================
// 統計関数（リスト関数）テスト
//==============================================================================

class ListFunctionTest : public ::testing::Test {};

TEST_F(ListFunctionTest, Sum) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 1 2 3 4 5 } sum"), 15.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 10 20 30 } sum"), 60.0);
}

TEST_F(ListFunctionTest, Product) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 1 2 3 4 5 } product"), 120.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 2 3 4 } product"), 24.0);
}

TEST_F(ListFunctionTest, Mean) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 2 4 6 8 10 } mean"), 6.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 1 2 3 } mean"), 2.0);
}

TEST_F(ListFunctionTest, Median) {
    // 奇数個
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 1 3 5 } median"), 3.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 5 1 3 } median"), 3.0);
    // 偶数個
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 1 2 3 4 } median"), 2.5);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 3 1 4 1 5 9 2 6 } median"), 3.5);
}

TEST_F(ListFunctionTest, Variance) {
    // 母分散: σ² = Σ(x - μ)² / n
    // {2, 4, 6, 8}: μ=5, σ² = ((2-5)² + (4-5)² + (6-5)² + (8-5)²) / 4 = (9+1+1+9)/4 = 5
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 2 4 6 8 } var"), 5.0);
}

TEST_F(ListFunctionTest, SampleVariance) {
    // 標本分散: s² = Σ(x - μ)² / (n-1)
    // {2, 4, 6, 8}: μ=5, s² = 20 / 3 ≈ 6.667
    EXPECT_NEAR(librpn::calculateRPN("{ 2 4 6 8 } svar"), 20.0 / 3.0, 1e-10);
}

TEST_F(ListFunctionTest, StandardDeviation) {
    // 母標準偏差: σ = √(σ²)
    // {2, 4, 6, 8}: σ = √5 ≈ 2.236
    EXPECT_NEAR(librpn::calculateRPN("{ 2 4 6 8 } stddev"), std::sqrt(5.0), 1e-10);
}

TEST_F(ListFunctionTest, SampleStandardDeviation) {
    // 標本標準偏差: s = √(s²)
    // {2, 4, 6, 8}: s = √(20/3) ≈ 2.582
    EXPECT_NEAR(librpn::calculateRPN("{ 2 4 6 8 } sstddev"), std::sqrt(20.0 / 3.0), 1e-10);
}

TEST_F(ListFunctionTest, MaxMin) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 3 1 4 1 5 9 2 6 } lmax"), 9.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 3 1 4 1 5 9 2 6 } lmin"), 1.0);
}

TEST_F(ListFunctionTest, Range) {
    // range = max - min
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 3 1 4 1 5 9 2 6 } range"), 8.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 10 20 30 } range"), 20.0);
}

TEST_F(ListFunctionTest, Count) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 1 2 3 4 5 } count"), 5.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN("{ 1 2 3 4 5 6 7 8 9 10 } count"), 10.0);
}

TEST_F(ListFunctionTest, CombinedWithOperators) {
    // mean + stddev
    double mean = 5.0;
    double stddev = std::sqrt(5.0);
    EXPECT_NEAR(librpn::calculateRPN("{ 2 4 6 8 } mean { 2 4 6 8 } stddev +"), mean + stddev, 1e-10);
}

//==============================================================================
// rpnToInfix テスト
//==============================================================================

class RPNToInfixTest : public ::testing::Test {};

TEST_F(RPNToInfixTest, BasicArithmetic) {
    EXPECT_EQ(librpn::rpnToInfix("1 2 +"), "(1 + 2)");
    EXPECT_EQ(librpn::rpnToInfix("3 4 -"), "(3 - 4)");
    EXPECT_EQ(librpn::rpnToInfix("5 6 *"), "(5 * 6)");
    EXPECT_EQ(librpn::rpnToInfix("8 2 /"), "(8 / 2)");
}

TEST_F(RPNToInfixTest, ComplexExpressions) {
    EXPECT_EQ(librpn::rpnToInfix("1 2 + 3 *"), "((1 + 2) * 3)");
    EXPECT_EQ(librpn::rpnToInfix("1 2 + 3 4 + *"), "((1 + 2) * (3 + 4))");
}

TEST_F(RPNToInfixTest, UnaryFunctions) {
    EXPECT_EQ(librpn::rpnToInfix("16 sqrt"), "sqrt(16)");
    EXPECT_EQ(librpn::rpnToInfix("16 sqrt 2 +"), "(sqrt(16) + 2)");
}

TEST_F(RPNToInfixTest, BinaryFunctions) {
    EXPECT_EQ(librpn::rpnToInfix("2 10 pow"), "pow(2, 10)");
    EXPECT_EQ(librpn::rpnToInfix("3 7 max"), "max(3, 7)");
}

TEST_F(RPNToInfixTest, RightAssociative) {
    EXPECT_EQ(librpn::rpnToInfix("2 3 2 ^ ^"), "(2 ^ (3 ^ 2))");
}

TEST_F(RPNToInfixTest, UnicodeOperators) {
    EXPECT_EQ(librpn::rpnToInfix("3 4 ×"), "(3 × 4)");
    EXPECT_EQ(librpn::rpnToInfix("8 2 ÷"), "(8 ÷ 2)");
}

//==============================================================================
// tokenize テスト
//==============================================================================

class TokenizeTest : public ::testing::Test {};

TEST_F(TokenizeTest, Numbers) {
    auto tokens = librpn::tokenize("123");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, librpn::TokenType::Number);
    EXPECT_EQ(tokens[0].value, "123");
}

TEST_F(TokenizeTest, DecimalNumbers) {
    auto tokens = librpn::tokenize("3.14");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, librpn::TokenType::Number);
    EXPECT_EQ(tokens[0].value, "3.14");
}

TEST_F(TokenizeTest, NegativeNumbers) {
    auto tokens = librpn::tokenize("-5");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, librpn::TokenType::Number);
    EXPECT_EQ(tokens[0].value, "-5");
}

TEST_F(TokenizeTest, Operators) {
    auto tokens = librpn::tokenize("1 + 2");
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[1].type, librpn::TokenType::Operator);
    EXPECT_EQ(tokens[1].value, "+");
}

TEST_F(TokenizeTest, UnaryFunctions) {
    auto tokens = librpn::tokenize("sqrt(16)");
    ASSERT_GE(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, librpn::TokenType::UnaryFunction);
    EXPECT_EQ(tokens[0].value, "sqrt");
}

TEST_F(TokenizeTest, BinaryFunctions) {
    auto tokens = librpn::tokenize("pow(2, 10)");
    ASSERT_GE(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, librpn::TokenType::BinaryFunction);
    EXPECT_EQ(tokens[0].value, "pow");
}

TEST_F(TokenizeTest, Constants) {
    auto tokens = librpn::tokenize("pi");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, librpn::TokenType::Constant);
    EXPECT_EQ(tokens[0].value, "pi");
}

TEST_F(TokenizeTest, UnicodeSymbols) {
    auto tokens = librpn::tokenize("π");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, librpn::TokenType::Constant);
    EXPECT_EQ(tokens[0].value, "π");
}

TEST_F(TokenizeTest, ListBrackets) {
    auto tokens = librpn::tokenize("{ 1 2 3 }");
    ASSERT_GE(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, librpn::TokenType::ListStart);
    EXPECT_EQ(tokens[4].type, librpn::TokenType::ListEnd);
}

//==============================================================================
// 判定関数テスト
//==============================================================================

class HelperFunctionTest : public ::testing::Test {};

TEST_F(HelperFunctionTest, IsOperator) {
    EXPECT_TRUE(librpn::isOperator("+"));
    EXPECT_TRUE(librpn::isOperator("-"));
    EXPECT_TRUE(librpn::isOperator("*"));
    EXPECT_TRUE(librpn::isOperator("/"));
    EXPECT_TRUE(librpn::isOperator("^"));
    EXPECT_TRUE(librpn::isOperator("×"));
    EXPECT_TRUE(librpn::isOperator("÷"));
    EXPECT_FALSE(librpn::isOperator("sqrt"));
    EXPECT_FALSE(librpn::isOperator("123"));
}

TEST_F(HelperFunctionTest, IsUnaryFunction) {
    EXPECT_TRUE(librpn::isUnaryFunction("sqrt"));
    EXPECT_TRUE(librpn::isUnaryFunction("sin"));
    EXPECT_TRUE(librpn::isUnaryFunction("cos"));
    EXPECT_TRUE(librpn::isUnaryFunction("abs"));
    EXPECT_TRUE(librpn::isUnaryFunction("√"));
    EXPECT_FALSE(librpn::isUnaryFunction("pow"));
    EXPECT_FALSE(librpn::isUnaryFunction("+"));
}

TEST_F(HelperFunctionTest, IsBinaryFunction) {
    EXPECT_TRUE(librpn::isBinaryFunction("pow"));
    EXPECT_TRUE(librpn::isBinaryFunction("max"));
    EXPECT_TRUE(librpn::isBinaryFunction("min"));
    EXPECT_TRUE(librpn::isBinaryFunction("atan2"));
    EXPECT_TRUE(librpn::isBinaryFunction("mod"));
    EXPECT_FALSE(librpn::isBinaryFunction("sqrt"));
    EXPECT_FALSE(librpn::isBinaryFunction("+"));
}

TEST_F(HelperFunctionTest, IsListFunction) {
    EXPECT_TRUE(librpn::isListFunction("sum"));
    EXPECT_TRUE(librpn::isListFunction("mean"));
    EXPECT_TRUE(librpn::isListFunction("median"));
    EXPECT_TRUE(librpn::isListFunction("stddev"));
    EXPECT_TRUE(librpn::isListFunction("var"));
    EXPECT_TRUE(librpn::isListFunction("lmax"));
    EXPECT_TRUE(librpn::isListFunction("lmin"));
    EXPECT_FALSE(librpn::isListFunction("sqrt"));
    EXPECT_FALSE(librpn::isListFunction("max"));
}

TEST_F(HelperFunctionTest, IsConstant) {
    EXPECT_TRUE(librpn::isConstant("pi"));
    EXPECT_TRUE(librpn::isConstant("PI"));
    EXPECT_TRUE(librpn::isConstant("e"));
    EXPECT_TRUE(librpn::isConstant("E"));
    EXPECT_TRUE(librpn::isConstant("π"));
    EXPECT_TRUE(librpn::isConstant("τ"));
    EXPECT_FALSE(librpn::isConstant("sqrt"));
    EXPECT_FALSE(librpn::isConstant("123"));
}

TEST_F(HelperFunctionTest, IsRightAssociative) {
    EXPECT_TRUE(librpn::isRightAssociative("^"));
    EXPECT_FALSE(librpn::isRightAssociative("+"));
    EXPECT_FALSE(librpn::isRightAssociative("-"));
    EXPECT_FALSE(librpn::isRightAssociative("*"));
    EXPECT_FALSE(librpn::isRightAssociative("/"));
}

TEST_F(HelperFunctionTest, GetPrecedence) {
    EXPECT_EQ(librpn::getPrecedence("+"), 1);
    EXPECT_EQ(librpn::getPrecedence("-"), 1);
    EXPECT_EQ(librpn::getPrecedence("*"), 2);
    EXPECT_EQ(librpn::getPrecedence("/"), 2);
    EXPECT_EQ(librpn::getPrecedence("%"), 2);
    EXPECT_EQ(librpn::getPrecedence("^"), 3);
    EXPECT_EQ(librpn::getPrecedence("×"), 2);
    EXPECT_EQ(librpn::getPrecedence("÷"), 2);
}

//==============================================================================
// 統合テスト（infixToRPN → calculateRPN）
//==============================================================================

class IntegrationTest : public ::testing::Test {};

TEST_F(IntegrationTest, BasicExpressions) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("1 + 2")), 3.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("2 * 3 + 4")), 10.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("(1 + 2) * 3")), 9.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("(1 + 2) * (3 + 4)")), 21.0);
}

TEST_F(IntegrationTest, PowerExpressions) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("2 ^ 3")), 8.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("2 ^ 3 ^ 2")), 512.0);
}

TEST_F(IntegrationTest, FunctionExpressions) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("sqrt(16)")), 4.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("sqrt(16) + 2")), 6.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("pow(2, 10)")), 1024.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("max(3, 7) + min(10, 5)")), 12.0);
}

TEST_F(IntegrationTest, NestedFunctions) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("sqrt(abs(-9))")), 3.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("pow(sqrt(16), 3)")), 64.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("max(pow(2, 3), min(10, 5))")), 8.0);
}

TEST_F(IntegrationTest, UnicodeExpressions) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("3 × 4 ÷ 2")), 6.0);
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("√(16)")), 4.0);
    EXPECT_NEAR(librpn::calculateRPN(librpn::infixToRPN("√(16) + π")), 4.0 + M_PI, 1e-10);
}

TEST_F(IntegrationTest, StatisticalFunctions) {
    EXPECT_DOUBLE_EQ(librpn::calculateRPN(librpn::infixToRPN("{ 10, 20, 30, 40, 50 } mean")), 30.0);
}
