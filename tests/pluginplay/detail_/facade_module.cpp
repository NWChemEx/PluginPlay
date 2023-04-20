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

    // Still not memoizable even if marked as memoizable (no UUID or Cache)
    f.turn_on_memoization();
    REQUIRE_FALSE(f.is_memoizable());
}
