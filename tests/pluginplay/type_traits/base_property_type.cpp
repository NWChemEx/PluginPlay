#include "pluginplay/type_traits/base_property_type.hpp"
#include <catch2/catch.hpp>
#include <type_traits>

// Declare some property types (they don't have to be instantiatable)
//{
using no_base =
  pluginplay::PropertyType<int, pluginplay::detail_::BasePropertyType>;

using a_base = pluginplay::PropertyType<int, no_base>;
//}

TEST_CASE("base_property_type_t") {
    SECTION("No base type") {
        using base_type = pluginplay::base_property_type_t<no_base>;
        using corr      = pluginplay::detail_::BasePropertyType;
        STATIC_REQUIRE(std::is_same_v<base_type, corr>);
    }
    SECTION("Base == no_base") {
        using base_type = pluginplay::base_property_type_t<a_base>;
        using corr      = no_base;
        STATIC_REQUIRE(std::is_same_v<base_type, corr>);
    }
}
