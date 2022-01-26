#include <catch2/catch_test_macros.hpp>
#include <token.h>

TEST_CASE("Token Builder Builds Correctly", "[token]") {

    auto token = trove::TokenBuilder::builder()
        .type(trove::Token::Type::IF)
        .position({0,1,2,3})
        .value("hello, world!")
        .build();

    REQUIRE(token.type==trove::Token::Type::IF);
    REQUIRE(token.source_position.index_start == 0);
    REQUIRE(token.source_position.index_end == 1);
    REQUIRE(token.source_position.line_start == 2);
    REQUIRE(token.source_position.line_end == 3);
    REQUIRE(token.value == "hello, world!");
}