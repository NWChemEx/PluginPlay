#include "sde/property_type/property_type.hpp"
#include "sde/type_traits/is_property_type.hpp"
#include <catch2/catch.hpp>
#include <type_traits>

// Declare some property types (they don't have to be instantiatable)
//{
DECLARE_PROPERTY_TYPE(no_base);
DECLARE_DERIVED_PROPERTY_TYPE(a_base, no_base);
//}

TEST_CASE("detail_::IsPropertyType") {
    SECTION("Defaulted base type") {
        SECTION("Not a property type") {
            using pt             = sde::detail_::IsPropertyType<int>;
            using base_prop_type = typename pt::base_prop_type;
            using BasePT         = sde::detail_::BasePropertyType;
            using corr           = sde::PropertyType<int, BasePT>;
            STATIC_REQUIRE(std::is_same_v<base_prop_type, corr>);
            STATIC_REQUIRE_FALSE(pt::value);
        }
        SECTION("Is a property type") {
            using pt             = sde::detail_::IsPropertyType<no_base>;
            using base_prop_type = typename pt::base_prop_type;
            using BasePT         = sde::detail_::BasePropertyType;
            using corr           = sde::PropertyType<no_base, BasePT>;
            STATIC_REQUIRE(std::is_same_v<base_prop_type, corr>);
            STATIC_REQUIRE(pt::value);
        }
    }
    SECTION("Specified base type") {
        SECTION("Not a property type") {
            using pt             = sde::detail_::IsPropertyType<int, no_base>;
            using base_prop_type = typename pt::base_prop_type;
            using corr           = sde::PropertyType<int, no_base>;
            STATIC_REQUIRE(std::is_same_v<base_prop_type, corr>);
            STATIC_REQUIRE_FALSE(pt::value);
        }
        SECTION("Is a property type") {
            using pt = sde::detail_::IsPropertyType<a_base, no_base>;
            using base_prop_type = typename pt::base_prop_type;
            using corr           = sde::PropertyType<a_base, no_base>;
            STATIC_REQUIRE(std::is_same_v<base_prop_type, corr>);
            STATIC_REQUIRE(pt::value);
        }
    }
}

using not_pts = std::tuple<int, std::vector<int>, std::string>;

TEMPLATE_LIST_TEST_CASE("is_property_type_v == false", "", not_pts) {
    STATIC_REQUIRE_FALSE(sde::is_property_type_v<TestType>);
}

TEST_CASE("is_property_type_v") {
    STATIC_REQUIRE(sde::is_property_type_v<no_base>);
}