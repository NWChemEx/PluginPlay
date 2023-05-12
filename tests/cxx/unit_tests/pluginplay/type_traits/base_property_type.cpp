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
