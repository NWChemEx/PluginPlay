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

#include "../../catch.hpp"
#include <pluginplay/any/any.hpp>
#include <pluginplay/fields/bounds_checking/bounds_checking.hpp>

using namespace pluginplay::bounds_checking;

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

TEST_CASE("TypeCheck") {
    using pluginplay::any::make_any_field;
    using vector_type = std::vector<int>;
    TypeCheck<int> c;
    REQUIRE(c(3));
    REQUIRE_FALSE(c("hello"));

    vector_type v{1, 2, 3};
    REQUIRE_FALSE(c(v));
    REQUIRE(c(make_any_field<int>(3)));
    REQUIRE_FALSE(c(make_any_field<std::string>("hello")));
    REQUIRE_FALSE(c(make_any_field<vector_type>(v)));
}
