#include <catch2/catch_test_macros.hpp>
#include <symtable.h>
#include <string>

TEST_CASE("Symtable Works", "[sym_table]") {

    /*
    SECTION("Put works") {
        auto sym_table = trove::SymTable<std::string>();
        sym_table.place("key", "value");
        auto value = sym_table.get("key");

        REQUIRE(value.has_value());
        REQUIRE(value.value() == "value");
    }*/

    /*SECTION("Scoped Put Works") {
        auto sym_table = trove::SymTable<std::string>();

        sym_table.enter();
        sym_table.enter();
        sym_table.enter();

        sym_table.place("key", "value");
        auto value = sym_table.get("key");
        auto level = sym_table.lookup_level("key");

        REQUIRE(value.has_value());
        REQUIRE(value.value() == "value");

        REQUIRE(level.has_value());
        REQUIRE(level.value() == 3);
    }*/
}