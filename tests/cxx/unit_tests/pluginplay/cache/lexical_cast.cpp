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

#include "lexical_cast.hpp"
#include <catch2/catch.hpp>

using namespace testing;

TEST_CASE("lexical_cast") {
    REQUIRE(lexical_cast<int>("42") == int{42});
    REQUIRE(lexical_cast<double>("42") == double{42});
    REQUIRE(lexical_cast<std::string>("42") == std::string{"42"});
}
