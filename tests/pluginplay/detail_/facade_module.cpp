#include "../test_common.hpp"
#include "pluginplay/facade_module.hpp"
#include <catch2/catch.hpp>

TEST_CASE("FacadeModule : single return") {
    auto f = pluginplay::make_facade<testing::OneOut>(2);
    REQUIRE(f.run_as<testing::OneOut>() == 2);
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

// Once uniques hashes are available for facade modules this unit test should be
// modified to require false for the hash comparison.
TEST_CASE("FacadeModule : different hashes for different facades") {
    auto f1 = pluginplay::make_facade<testing::OneOut>(1);
    auto f2 = pluginplay::make_facade<testing::OneOut>(2);
    REQUIRE(pluginplay::hash_objects(f1) == pluginplay::hash_objects(f2));
}
