/*
 * Copyright 2023 NWChemEx-Project
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

#include <catch2/catch.hpp>
#include <pluginplay/python/python_wrapper.hpp>

using namespace pluginplay::python;

#ifdef BUILD_PYBIND11

TEST_CASE("PythonWrapper  -- BUILD_PYBIND11 is defined") {
    pybind11::object null_obj;
    PythonWrapper holds_null(null_obj);

    SECTION("CTors") {
        SECTION("value ctor") {
            REQUIRE_FALSE(holds_null.has_value());
            // REQUIRE(holds_value.has_value());
        }
    }

    SECTION("has_value") {
        REQUIRE_FALSE(holds_null.has_value());
        // REQUIRE(holds_value.has_value());
    }
}

#else

TEST_CASE("PythonWrapper -- BUILD_PYBIND11 not defined") {
    REQUIRE_THROWS_AS(PythonWrapper(), std::runtime_error);
}

#endif
