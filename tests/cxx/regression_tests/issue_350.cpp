/*
 * Copyright 2024 NWChemEx Community
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
#include <catch2/catch.hpp>

namespace {
struct FromClass {
    int x = 0;

    bool operator==(const FromClass& rhs) const { return x == rhs.x; }
};

struct ToClass {
    ToClass(FromClass input) : x(input.x + 1) {}

    bool operator==(const ToClass& rhs) const { return x == rhs.x; }

    int x = 0;
};

DECLARE_PROPERTY_TYPE(ImplicitConvert);
PROPERTY_TYPE_INPUTS(ImplicitConvert) {
    return pluginplay::declare_input().add_field<ToClass>(
      "Class we can implicitly convert to");
}
PROPERTY_TYPE_RESULTS(ImplicitConvert) { return pluginplay::declare_result(); }

} // namespace

/* There used to be a bug where if your PT took an object of type ToClass and
 * the user passed in an object for FromClass (which is implicitly convertible
 * to an object of type ToClass) then when you use the property type to wrap
 * the FromClass object you fail the runtime bounds check because wrap_
 * forwarded the FromClass object into the ModuleInput as a FromClass object
 * and not a ToClass object.
 *
 * If you couldn't follow that then just know the following test used to fail...
 */
TEST_CASE("Issue 350") {
    FromClass x{1};

    auto rv    = ImplicitConvert::inputs();
    rv         = ImplicitConvert::wrap_inputs(rv, x);
    auto value = rv.at("Class we can implicitly convert to").value<ToClass>();
    REQUIRE(value.x == 2);
}
