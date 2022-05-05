#include "../test_common.hpp"
#include "pluginplay/facade_module.hpp"
#include <catch2/catch.hpp>

TEST_CASE("FacadeModule : single return") {
    auto f = pluginplay::make_facade<testing::OneOut>(2);
    REQUIRE(std::get<0>(f.run_as<testing::OneOut>()) == 2);
}

TEST_CASE("FacadeModule : multiple returns") {
    auto f      = pluginplay::make_facade<testing::TwoOut>(2, 'b');
    auto [i, c] = f.run_as<testing::TwoOut>();
    REQUIRE(i == 2);
    REQUIRE(c == 'b');
}

TEST_CASE("FacadeModule : is_memoizable") {
    auto f = pluginplay::make_facade<testing::OneOut>(2);
    REQUIRE_FALSE(f.is_memoizable());
    f.turn_on_memoization();
    REQUIRE(f.is_memoizable());
}
