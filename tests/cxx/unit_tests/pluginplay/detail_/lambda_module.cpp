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
#include "pluginplay/lambda_module.hpp"
#include <catch2/catch.hpp>

/* Testing strategy.
 *
 * For the most part the only moving part of the LambdaModule class is the
 * wrapping/unwrapping of the inputs/results to the user-provided function. The
 * unwrapping process defers to the PropertyType class and is thus tested there.
 * For the wrapping process we have to do a bit of template meta-programming
 * depending on whether we are wrapping one or multiple returns. This is what we
 * test in this source file
 */

TEST_CASE("LambdaModule") {
    SECTION("single return") {
        auto l = pluginplay::make_lambda<testing::OneOut>([]() { return 2; });
        REQUIRE(l.run_as<testing::OneOut>() == 2);
    }

    SECTION("multiple returns") {
        auto l = pluginplay::make_lambda<testing::TwoOut>(
          []() { return std::make_tuple(2, 'b'); });
        auto [i, c] = l.run_as<testing::TwoOut>();
        REQUIRE(i == 2);
        REQUIRE(c == 'b');
    }
}
