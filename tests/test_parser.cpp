#include <catch2/catch_test_macros.hpp>
#include <lexer.h>
#include <parser.h>

TEST_CASE("Types match", "[type]") {

    auto source = "x u32 = 123";
    auto compilation_unit = trove::CompilationUnit(source);
    auto err_reporter = trove::ErrorReporter(compilation_unit);
    auto lexer = trove::Lexer(err_reporter, source);
    auto tokens = lexer.lex();
    auto parser = trove::Parser(compilation_unit, tokens);
    auto ast = parser.parse();

    REQUIRE(ast);
}