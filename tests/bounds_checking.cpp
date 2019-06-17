#include <catch2/catch.hpp>
#include <sde/bounds_checking.hpp>

using namespace sde::bounds_checking;

TEST_CASE("NotEqualTo") {
    NotEqualTo<int> c(3);
    REQUIRE(c(4));
    REQUIRE_FALSE(c(3));
    REQUIRE(c.str() == "!= 3");
    REQUIRE(print_bounds(c) == "!= 3");
}

TEST_CASE("GreaterThan") {
    GreaterThan<int> c(3);
    REQUIRE(c(4));
    REQUIRE_FALSE(c(3));
    REQUIRE_FALSE(c(2));
    REQUIRE(c.str() == "> 3");
    REQUIRE(print_bounds(c) == "> 3");
}

TEST_CASE("LessThan") {
    LessThan<int> c(3);
    REQUIRE_FALSE(c(4));
    REQUIRE_FALSE(c(3));
    REQUIRE(c(2));
    REQUIRE(c.str() == "< 3");
    REQUIRE(print_bounds(c) == "< 3");
}

TEST_CASE("GreaterThanEqualTo") {
    GreaterThanEqualTo<int> c(3);
    REQUIRE(c(4));
    REQUIRE(c(3));
    REQUIRE_FALSE(c(2));
    REQUIRE(c.str() == ">= 3");
    REQUIRE(print_bounds(c) == ">= 3");
}

TEST_CASE("LessThanEqualTo") {
    LessThanEqualTo<int> c(3);
    REQUIRE_FALSE(c(4));
    REQUIRE(c(3));
    REQUIRE(c(2));
    REQUIRE(c.str() == "<= 3");
    REQUIRE(print_bounds(c) == "<= 3");
}

TEST_CASE("InRange") {
    InRange<int> c(3);
    REQUIRE_FALSE(c(4));
    REQUIRE_FALSE(c(3));
    REQUIRE(c(2));
    REQUIRE(c(0));
    REQUIRE_FALSE(c(-1));
    REQUIRE(c.str() == "in [0, 3)");
    REQUIRE(print_bounds(c) == "in [0, 3)");
}
