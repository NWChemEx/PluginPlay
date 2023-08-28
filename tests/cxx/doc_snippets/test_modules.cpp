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

#include "electric_field.hpp"
#include "load_modules.hpp"
#include "modules.hpp"
#include <catch2/catch.hpp>

using namespace pluginplay_examples;
TEST_CASE("Tutorial modules") {
    // Set up module manager
    pluginplay::ModuleManager mm;
    // Load your modules
    load_modules(mm);
    REQUIRE(mm.count("Force") == 1);
    REQUIRE(mm.count("Coulomb's Law") == 1);
    REQUIRE(mm.count("Coulomb's Law with screening") == 1);
    REQUIRE(mm.count("Single-precision Coulomb's law") == 1);
    REQUIRE(mm.count("Nonexistent module") == 0);
    auto force_mod = mm.at("Coulomb's Law");
    Point p0{0.0, 0.0, 0.0};
    Point p1{1.0, 0.0, 0.0};
    Point p2{2.0, 0.0, 0.0};
    Point r1{1.5, 0.0, 0.0};
    PointCharge pc1{1.0, p1};
    PointCharge pc2{2.0, p2};
    std::vector pcs{pc1, pc2};
    auto result = force_mod.run_as<ElectricField>(p0, pcs);
    REQUIRE(result == r1);
}
