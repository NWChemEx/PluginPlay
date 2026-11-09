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

#include "drivers.hpp"
#include "force.hpp"
#include "pluginplay_examples.hpp"
#include <catch2/catch_test_macros.hpp>
#include <limits>

using namespace pluginplay_examples;

TEST_CASE("Using a driver") {
    // A particle with charge 1 at the origin, and two point charges
    PointCharge q{1.0, Point{0.0, 0.0, 0.0}};
    double m = 2.0;
    Point a{2.0, 0.0, 0.0};
    std::vector<PointCharge> charges{PointCharge{1.0, Point{1.0, 0.0, 0.0}},
                                     PointCharge{2.0, Point{2.0, 0.0, 0.0}}};

    pluginplay::ModuleManager mm;
    mm.load_plugin<PluginplayExamples>();
    mm.add_module<ForceDriver>("Force driver");
    mm.change_submod("Force driver", "force", "Classical Force");

    // Run with the default settings
    auto F = mm.run_as<Force>("Force driver", q, m, a, charges);

    REQUIRE(F == Point{5.5, 0.0, 0.0});
    REQUIRE(mm.at("Force driver").inactive_settings().empty());

    // Use other settings by copying the driver
    mm.copy_module("Force driver", "Screened force driver");
    mm.change_input("Screened force driver", "screen", true);
    mm.change_input("Screened force driver", "threshold", 1.5);
    auto F_screened =
      mm.run_as<Force>("Screened force driver", q, m, a, charges);

    REQUIRE(F_screened == Point{5.0, 0.0, 0.0});
    REQUIRE(mm.at("Screened force driver").inactive_settings().empty());

    // The ModuleManager's modules are not changed by the driver
    const auto& efield =
      mm.at("Classical Force").submods().at("electric field");
    REQUIRE(efield.value().get_name() == "Coulomb's Law");
    const auto& threshold =
      mm.at("Coulomb's Law with screening").inputs().at("threshold");
    REQUIRE(threshold.value<double>() == std::numeric_limits<double>::max());
    REQUIRE(mm.run_as<Force>("Force driver", q, m, a, charges) == F);

    // Change "threshold" without turning on "screen"
    mm.copy_module("Force driver", "Unscreened force driver");
    mm.change_input("Unscreened force driver", "threshold", 1.5);
    mm.run_as<Force>("Unscreened force driver", q, m, a, charges);

    auto inactive = mm.at("Unscreened force driver").inactive_settings();
    // "threshold" is inactive: the branch that forwards it was not taken

    REQUIRE(inactive.size() == 1);
    REQUIRE(inactive[0] == "\"Unscreened force driver\": \"threshold\" has no "
                           "effect because no branch that forwards it was "
                           "taken");

    // Override one of the driver's choices
    mm.copy_module("Force driver", "My force driver");
    auto& request = mm.at("My force driver").call_graph_request();
    request.set_submod("Classical Force", "electric field",
                       "Coulomb's Law with screening");
    request.set_input("Coulomb's Law with screening", "threshold", 1.5);
    auto F_mine = mm.run_as<Force>("My force driver", q, m, a, charges);

    REQUIRE(F_mine == Point{5.0, 0.0, 0.0});
}

TEST_CASE("Wiring a driver's submodule") {
    PointCharge q{1.0, Point{0.0, 0.0, 0.0}};
    double m = 2.0;
    Point a{2.0, 0.0, 0.0};
    std::vector<PointCharge> charges{PointCharge{1.0, Point{1.0, 0.0, 0.0}},
                                     PointCharge{2.0, Point{2.0, 0.0, 0.0}}};

    pluginplay::ModuleManager mm;
    mm.load_plugin<PluginplayExamples>();
    mm.change_input("Coulomb's Law with screening", "threshold", 1.5);
    mm.add_module<FieldChoiceDriver>("Field choice driver");

    // The defaults of the driver's submodules are used
    REQUIRE(mm.run_as<Force>("Field choice driver", q, m, a, charges) ==
            Point{5.5, 0.0, 0.0});

    // The submodule the user binds is passed on to "Classical Force"
    mm.copy_module("Field choice driver", "Screened field choice driver");
    mm.change_submod("Screened field choice driver", "electric field",
                     "Coulomb's Law with screening");
    REQUIRE(mm.run_as<Force>("Screened field choice driver", q, m, a,
                             charges) == Point{5.0, 0.0, 0.0});
}
