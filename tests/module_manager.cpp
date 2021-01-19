#include "sde/module_manager.hpp"
#include "test_common.hpp"
#include <catch2/catch.hpp>

TEST_CASE("ModuleManager") {
    sde::ModuleManager mm;

    SECTION("add_module(string)") {
        using mod_t = testing::NoPTModule; // Type of the Module we're adding

        mm.add_module<mod_t>("a mod");
        auto corr = testing::make_module<mod_t>();
        REQUIRE(mm.at("a mod") == *corr);
    }

    SECTION("add_module(string, shared_ptr") {
        using mod_t = testing::NoPTModule; // Type of the Module we're adding

        mm.add_module("a mod", std::make_shared<mod_t>());
        auto corr = testing::make_module<mod_t>();
        REQUIRE(mm.at("a mod") == *corr);
    }
}
