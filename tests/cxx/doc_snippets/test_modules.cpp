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
#include "force.hpp"
#include "load_modules.hpp"
#include "modules.hpp"
#include <catch2/catch.hpp>

using namespace pluginplay_examples;
TEST_CASE("Tutorial modules") {
    // Set up module manager
    pluginplay::ModuleManager mm;
    // Load your modules
    load_modules(mm);
    REQUIRE(mm.count("Classical Force") == 1);
    REQUIRE(mm.count("Coulomb's Law") == 1);
    REQUIRE(mm.count("Coulomb's Law with screening") == 1);
    REQUIRE(mm.count("Single-precision Coulomb's law") == 1);
    REQUIRE(mm.count("Nonexistent module") == 0);

    auto force0_mod = mm.at("Classical Force");
    auto force1_mod = mm.at("Coulomb's Law");
    auto force2_mod = mm.at("Single-precision Coulomb's law");
    auto force3_mod = mm.at("Coulomb's Law with screening");
    Point p0{0.0, 0.0, 0.0};
    Point p1{1.0, 0.0, 0.0};
    Point p2{2.0, 0.0, 0.0};
    Point r0{5.5, 0.0, 0.0};
    Point r1{1.5, 0.0, 0.0};
    PointCharge pc0{1.0, p0};
    PointCharge pc1{1.0, p1};
    PointCharge pc2{2.0, p2};
    std::vector pcs{pc1, pc2};
    double m1{2.0};

    auto result0 = force0_mod.run_as<Force>(pc0, m1, p2, pcs);
    REQUIRE(result0 == r0);

    auto result1 = force1_mod.run_as<ElectricField>(p0, pcs);
    REQUIRE(result1 == r1);

    auto result2 = force2_mod.run_as<ElectricField>(p0, pcs);
    REQUIRE(result2 == r1);

    auto result3 = force3_mod.run_as<ElectricField>(p0, pcs);
    REQUIRE(result3 == r1);

    auto force3_mod_copy = force3_mod.unlocked_copy();
    force3_mod_copy.change_input("threshold", 1.0);
    auto screened_result3 = force3_mod_copy.run_as<ElectricField>(p0, pcs);
    REQUIRE(screened_result3 == p0);
}
