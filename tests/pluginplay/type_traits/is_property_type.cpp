/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pluginplay/property_type/property_type.hpp"
#include "pluginplay/type_traits/is_property_type.hpp"
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
            using pt             = pluginplay::detail_::IsPropertyType<int>;
            using base_prop_type = typename pt::base_prop_type;
            using BasePT         = pluginplay::detail_::BasePropertyType;
            using corr           = pluginplay::PropertyType<int, BasePT>;
            STATIC_REQUIRE(std::is_same_v<base_prop_type, corr>);
            STATIC_REQUIRE_FALSE(pt::value);
        }
        SECTION("Is a property type") {
            using pt             = pluginplay::detail_::IsPropertyType<no_base>;
            using base_prop_type = typename pt::base_prop_type;
            using BasePT         = pluginplay::detail_::BasePropertyType;
            using corr           = pluginplay::PropertyType<no_base, BasePT>;
            STATIC_REQUIRE(std::is_same_v<base_prop_type, corr>);
            STATIC_REQUIRE(pt::value);
        }
    }
    SECTION("Specified base type") {
        SECTION("Not a property type") {
            using pt = pluginplay::detail_::IsPropertyType<int, no_base>;
            using base_prop_type = typename pt::base_prop_type;
            using corr           = pluginplay::PropertyType<int, no_base>;
            STATIC_REQUIRE(std::is_same_v<base_prop_type, corr>);
            STATIC_REQUIRE_FALSE(pt::value);
        }
        SECTION("Is a property type") {
            using pt = pluginplay::detail_::IsPropertyType<a_base, no_base>;
            using base_prop_type = typename pt::base_prop_type;
            using corr           = pluginplay::PropertyType<a_base, no_base>;
            STATIC_REQUIRE(std::is_same_v<base_prop_type, corr>);
            STATIC_REQUIRE(pt::value);
        }
    }
}

using not_pts = std::tuple<int, std::vector<int>, std::string>;

TEMPLATE_LIST_TEST_CASE("is_base_property_type_v == false", "", not_pts) {
    STATIC_REQUIRE_FALSE(pluginplay::is_base_property_type_v<TestType>);
}

TEST_CASE("is_property_type_v") {
    STATIC_REQUIRE(pluginplay::is_base_property_type_v<no_base>);
}
