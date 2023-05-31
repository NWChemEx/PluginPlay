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

/** Testing Notes.
 *
 *  Without a Python interpreter running we can not actually create a Python
 *  object. Thus this test case will only test the behavior of the null object.
 *  The corresponding Python unit test will actually create Python objects and
 *  pass them into PythonWrappers.
 */
TEST_CASE("PythonWrapper  -- BUILD_PYBIND11 is defined") {
    pybind11::object null_obj;
    PythonWrapper holds_null(null_obj);

    SECTION("CTors") {
        SECTION("value ctor") { REQUIRE_FALSE(holds_null.has_value()); }

        SECTION("copy ctor") {
            PythonWrapper null2(holds_null);
            REQUIRE(holds_null == null2);
        }

        SECTION("move ctor") {
            PythonWrapper copied(holds_null);
            PythonWrapper moved(std::move(holds_null));
            REQUIRE(moved == copied);
        }

        // With pure C++ can't prepare a non-null state to test assignment
    }

    SECTION("has_value") { REQUIRE_FALSE(holds_null.has_value()); }

    SECTION("is_convertible") {
        REQUIRE_FALSE(holds_null.is_convertible<pybind11::object>());
        REQUIRE_FALSE(holds_null.is_convertible<int>());
    }

    SECTION("is_convertible() const") {
        REQUIRE_FALSE(
          std::as_const(holds_null).is_convertible<pybind11::object>());
        REQUIRE_FALSE(std::as_const(holds_null).is_convertible<int>());
    }

    SECTION("unwrap") {
        REQUIRE_THROWS_AS(holds_null.unwrap<pybind11::object>(),
                          std::runtime_error);
        REQUIRE_THROWS_AS(holds_null.unwrap<int>(), std::runtime_error);
    }

    SECTION("unwrap() const") {
        REQUIRE_THROWS_AS(std::as_const(holds_null).unwrap<pybind11::object>(),
                          std::runtime_error);
        REQUIRE_THROWS_AS(std::as_const(holds_null).unwrap<int>(),
                          std::runtime_error);
    }

    SECTION("Comparisons") {
        PythonWrapper null2(holds_null);
        REQUIRE(holds_null == null2);
        REQUIRE_FALSE(holds_null != null2);
    }

    SECTION("as_string") { REQUIRE(holds_null.as_string() == ""); }
}

#else

TEST_CASE("PythonWrapper -- BUILD_PYBIND11 not defined") {
    REQUIRE_THROWS_AS(PythonWrapper(), std::runtime_error);
}

#endif
