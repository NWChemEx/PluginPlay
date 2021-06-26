#include "sde/facade_module.hpp"
#include "tests/test_common.hpp"
#include <catch2/catch.hpp>

TEST_CASE("FacadeModule : single return") {
    auto f = sde::make_facade<testing::OneOut>(2);
    REQUIRE(std::get<0>(f.run_as<testing::OneOut>()) == 2);
}

TEST_CASE("FacadeModule : multiple returns") {
    auto f      = sde::make_facade<testing::TwoOut>(2, 'b');
    auto [i, c] = f.run_as<testing::TwoOut>();
    REQUIRE(i == 2);
    REQUIRE(c == 'b');
}

TEST_CASE("FacadeModule : is_memoizable") {
    auto f = sde::make_facade<testing::OneOut>(2);
    REQUIRE_FALSE(f.is_memoizable());
    f.turn_on_memoization();
    REQUIRE(f.is_memoizable());
}

// Once uniques hashes are available for facade modules this unit test should be
// modified to require false for the hash comparison.
TEST_CASE("FacadeModule : different hashes for different facades") {
    auto f1 = sde::make_facade<testing::OneOut>(1);
    auto f2 = sde::make_facade<testing::OneOut>(2);
    REQUIRE(runtime::hash_objects(f1) == runtime::hash_objects(f2));
}