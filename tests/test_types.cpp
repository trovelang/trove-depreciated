#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <type.h>

int main(int argc, const int *const *argv){
    
    int result = Catch::Session().run( argc, argv );


    auto type1 = trove::TypeBuilder::builder()
        .base_type(trove::Type::BaseType::S32)
        .build();

    auto type2 = trove::TypeBuilder::builder()
        .base_type(trove::Type::BaseType::S32)
        .build();


    // REQUIRE(type1.equals(type2));

    REQUIRE(1==1);

    return 0;
}