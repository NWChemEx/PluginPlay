#include "sde/property_type/traits.hpp"
#include <catch2/catch.hpp>
#include <type_traits>

// Declare some property types (they don't have to be instantiatable)
using no_base = sde::PropertyType<int, sde::detail_::BasePropertyType>;

using a_base = sde::PropertyType<int, no_base>;

TEST_CASE("base_property_type_t") {
    SECTION("No base type") {
        using base_type = sde::base_property_type_t<no_base>;
        using corr      = sde::detail_::BasePropertyType;
        STATIC_REQUIRE(std::is_same_v<base_type, corr>);
    }
    SECTION("Base == no_base") {
        using base_type = sde::base_property_type_t<a_base>;
        using corr      = no_base;
        STATIC_REQUIRE(std::is_same_v<base_type, corr>);
    }
}

TEST_CASE("is_base_property_type_v") {
    SECTION("No base type") {
        STATIC_REQUIRE(sde::is_base_property_type_v<no_base>);
    }
    SECTION("Base == no_base") {
        STATIC_REQUIRE_FALSE(sde::is_base_property_type_v<a_base>);
    }
}