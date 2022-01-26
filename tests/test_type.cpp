#include <catch2/catch_test_macros.hpp>
#include <type.h>

TEST_CASE("Types match", "[type]"){
    
    auto type1 = trove::TypeBuilder::builder()
        .base_type(trove::Type::BaseType::S32)
        .build();

    auto type2 = trove::TypeBuilder::builder()
        .base_type(trove::Type::BaseType::S32)
        .build();

    REQUIRE(type1.equals(type2));
}

TEST_CASE("Types do not match", "[type]") {

    auto type1 = trove::TypeBuilder::builder()
        .base_type(trove::Type::BaseType::S32)
        .build();

    auto type2 = trove::TypeBuilder::builder()
        .base_type(trove::Type::BaseType::FN)
        .build();

    REQUIRE_FALSE(type1.equals(type2));
}