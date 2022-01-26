#include <catch2/catch_test_macros.hpp>
#include <lexer.h>

TEST_CASE("Correct Lexing From Source", "[type]") {

    auto source = "x u32 = 123";
    auto compilation_unit = trove::CompilationUnit(source);
    auto err_reporter = trove::ErrorReporter(compilation_unit);
    auto lexer = trove::Lexer(err_reporter, source);
    auto tokens = lexer.lex();

    REQUIRE(tokens.size() == 5);
    REQUIRE(tokens[0].type == trove::Token::Type::IDENTIFIER);
    REQUIRE(tokens[0].value == "x");
    REQUIRE(tokens[1].type == trove::Token::Type::U32);
    REQUIRE(tokens[2].type == trove::Token::Type::ASSIGN);
    REQUIRE(tokens[3].type == trove::Token::Type::NUM);
    REQUIRE(tokens[3].value == "123");
}