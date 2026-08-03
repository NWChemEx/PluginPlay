/*
 * Copyright 2026 NWChemEx-Project
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

#include "../catch.hpp"
#include <pluginplay/property_type/python_only_property_type.hpp>

using namespace pluginplay;
using namespace pluginplay::python;

/** Testing Notes.
 *
 *  Without a running Python interpreter we can not safely create live Python
 *  objects (or a non-empty pybind11::args instance -- default-constructing
 *  one is safe, but calling methods like size() on it is not). This test
 *  therefore focuses on the parts of PythonOnlyPropertyType that don't
 *  require live Python values: name(), field declaration, and the
 *  inputs()/results()/input_order()/result_order() accessors. The
 *  wrap_inputs/wrap_results/unwrap_inputs/unwrap_results round trip (which
 *  needs real Python objects) is exercised by the corresponding Python unit
 *  test.
 */
TEST_CASE("PythonOnlyPropertyType") {
    PythonOnlyPropertyType pt("PydanticAPI");

    SECTION("name") { REQUIRE(pt.name() == "PydanticAPI"); }

    SECTION("no fields by default") {
        REQUIRE(pt.inputs().empty());
        REQUIRE(pt.results().empty());
        REQUIRE(pt.input_order().empty());
        REQUIRE(pt.result_order().empty());
    }

    SECTION("declare_input") {
        auto& in = pt.declare_input("input schema");
        in.set_description("A pydantic BaseModel instance");

        REQUIRE(pt.inputs().count("input schema") == 1);
        REQUIRE(pt.input_order() == std::vector<type::key>{"input schema"});
        REQUIRE(pt.inputs().at("input schema").description() ==
                "A pydantic BaseModel instance");

        SECTION("multiple inputs preserve declaration order") {
            pt.declare_input("another input");
            REQUIRE(pt.input_order() ==
                    std::vector<type::key>{"input schema", "another input"});
        }
    }

    SECTION("declare_result") {
        auto& r = pt.declare_result("result schema");
        r.set_description("A pydantic BaseModel instance");

        REQUIRE(pt.results().count("result schema") == 1);
        REQUIRE(pt.result_order() == std::vector<type::key>{"result schema"});
        REQUIRE(pt.results().at("result schema").description() ==
                "A pydantic BaseModel instance");
    }
}
