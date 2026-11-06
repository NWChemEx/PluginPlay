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
#include "driver_test_modules.hpp"

using namespace testing::drivers;
using pluginplay::QualifiedKey;

TEST_CASE("Module: driver API") {
    auto mm = make_mm();

    SECTION("is_driver") {
        REQUIRE(mm.at("X driver").is_driver());
        REQUIRE_FALSE(mm.at("Wrapper").is_driver());
    }

    SECTION("call_graph_request") {
        auto& request = mm.at("X driver").call_graph_request();
        REQUIRE(request.empty());
        request.set_input("Leaf", "x", 3);
        REQUIRE(mm.at("X driver").call_graph_request().requests().size() == 1);

        REQUIRE_THROWS_AS(mm.at("Wrapper").call_graph_request(),
                          std::runtime_error);

        run(mm, "X driver");
        REQUIRE_THROWS_AS(mm.at("X driver").call_graph_request(),
                          std::runtime_error);
    }

    SECTION("inactive_settings") {
        REQUIRE(mm.at("X driver").inactive_settings().empty());
        REQUIRE(mm.at("Wrapper").inactive_settings().empty());
    }

    SECTION("The developer's request") {
        XDriver base;
        const auto& requests = base.call_graph_request().requests();
        REQUIRE(requests.size() == 1);
        REQUIRE(requests[0].source_key() == "x");
        REQUIRE(requests[0].module_key() == QualifiedKey("Leaf"));
        REQUIRE(base.call_graph_request().submod_defaults().at("root") ==
                QualifiedKey("Wrapper"));
        REQUIRE(Leaf{}.call_graph_request().empty());
    }
}

TEST_CASE("Drivers: must be run through a ModuleManager") {
    auto mm = make_mm();
    REQUIRE_THROWS_AS(mm.at("X driver").run_as<Value>(), std::runtime_error);

    // Once the ModuleManager resolved its requests, it can be run directly
    REQUIRE(run(mm, "X driver") == 2);
    REQUIRE(mm.at("X driver").run_as<Value>() == 2);
}

TEST_CASE("Drivers: forwarding an input") {
    auto mm = make_mm();
    mm.copy_module("X driver", "X driver 5");
    mm.change_input("X driver 5", "x", 5);
    REQUIRE(run(mm, "X driver 5") == 6);
    REQUIRE(mm.at("X driver 5").inactive_settings().empty());

    SECTION("The ModuleManager's modules are not changed") {
        REQUIRE(mm.at("Leaf").inputs().at("x").value<int>() == 1);
        // N.B. "Wrapper" shares its cache with the driver's overlay of it, so
        // this fails if memoization can not tell them apart
        REQUIRE(run(mm, "Wrapper") == 2);
    }

    SECTION("Running the driver locks the modules below it") {
        REQUIRE(mm.at("X driver 5").locked());
        REQUIRE(mm.at("Leaf").locked());
        REQUIRE(mm.at("Wrapper").locked());
    }

    SECTION("Copies of a locked driver resolve their requests again") {
        mm.copy_module("X driver 5", "X driver 10");
        mm.change_input("X driver 10", "x", 10);
        REQUIRE(run(mm, "X driver 10") == 11);
        REQUIRE(run(mm, "X driver 5") == 6);
    }

    SECTION("A module bound in two places gets one overlay") {
        auto mm2 = make_mm();
        mm2.change_submod("Adder", "b", "Leaf");
        mm2.change_input("X driver", "x", 5);
        REQUIRE(run(mm2, "X driver") == 10);
    }
}

TEST_CASE("Drivers: settings with no effect") {
    auto mm = make_mm();
    // The user swaps out the module whose input the driver sets
    mm.change_submod("X driver", "root", "Other leaf");

    SECTION("Inactive settings are recorded") {
        REQUIRE(run(mm, "X driver") == 1);
        auto inactive = mm.at("X driver").inactive_settings();
        REQUIRE(inactive.size() == 1);
        REQUIRE(inactive[0] ==
                "\"X driver\": forwarding input \"x\" to \"Leaf\"'s input "
                "\"x\" has no effect because \"Leaf\" is not in the call "
                "graph");
    }

    SECTION("Required settings throw") {
        mm.change_submod("Required X driver", "root", "Other leaf");
        REQUIRE_THROWS_AS(run(mm, "Required X driver"), std::runtime_error);
    }

    SECTION("A module whose plugin is not loaded") {
        mm.change_submod("X driver", "root", "Wrapper");
        mm.at("X driver")
          .call_graph_request()
          .set_input(QualifiedKey("Not loaded", "Leaf"), "x", 3);
        REQUIRE(run(mm, "X driver") == 2);
        auto inactive = mm.at("X driver").inactive_settings();
        REQUIRE(inactive.size() == 1);
        REQUIRE(inactive[0].find("plugin \"Not loaded\" is not loaded") !=
                std::string::npos);
    }

    SECTION("A module removed by a later setting") {
        auto& request = mm.at("Chain driver").call_graph_request();
        request.set_submod("Wrapper", "inner", "Leaf");
        REQUIRE(run(mm, "Chain driver") == 1);
        auto inactive = mm.at("Chain driver").inactive_settings();
        REQUIRE(inactive.size() == 1);
        REQUIRE(inactive[0].find("was removed from the call graph") !=
                std::string::npos);
    }
}

TEST_CASE("Drivers: errors") {
    auto mm = make_mm();

    SECTION("Unknown module") {
        REQUIRE_THROWS_AS(run(mm, "Unknown module driver"), std::out_of_range);
    }

    SECTION("Unknown input of a module in the call graph") {
        REQUIRE_THROWS_AS(run(mm, "Unknown input driver"), std::out_of_range);
    }

    SECTION("Two keys for the same module") {
        REQUIRE_THROWS_AS(run(mm, "Same module driver"), std::invalid_argument);
    }

    SECTION("A module below a nested driver") {
        REQUIRE_THROWS_AS(run(mm, "Encapsulation driver"), std::runtime_error);
    }

    SECTION("A failed run leaves the driver usable") {
        // "b" has no default, so the driver is not ready
        REQUIRE_THROWS_AS(run(mm, "Submod driver"), std::runtime_error);
        REQUIRE_FALSE(mm.at("Submod driver").locked());
        mm.copy_module("Leaf", "Ten");
        mm.change_input("Ten", "x", 10);
        mm.change_submod("Submod driver", "b", "Ten");
        REQUIRE(run(mm, "Submod driver") == 11);
    }
}

TEST_CASE("Drivers: one owner per target") {
    using pluginplay::DriverBase;

    struct TwoTargets : DriverBase {
        TwoTargets() : DriverBase(this) {
            add_input<int>("x").set_target("Leaf", "x");
            add_input<int>("y").set_target("Leaf", "x");
        }
        result_map_type run_(input_map_type,
                             submodule_map_type) const override {
            return results();
        }
    };

    struct TargetAndChain : DriverBase {
        TargetAndChain() : DriverBase(this) {
            add_input<int>("x").set_target("Leaf", "x");
            add_input<bool>("flag");
            when("flag", true).set_input("Leaf", "x", 2);
        }
        result_map_type run_(input_map_type,
                             submodule_map_type) const override {
            return results();
        }
    };

    struct OneChain : DriverBase {
        OneChain() : DriverBase(this) {
            add_input<bool>("flag");
            when("flag", true)
              .set_input("Leaf", "x", 2)
              .otherwise()
              .set_input("Leaf", "x", 3);
        }
        result_map_type run_(input_map_type,
                             submodule_map_type) const override {
            return results();
        }
    };

    REQUIRE_THROWS_AS(TwoTargets{}, std::invalid_argument);
    REQUIRE_THROWS_AS(TargetAndChain{}, std::invalid_argument);
    REQUIRE_NOTHROW(OneChain{});
}

TEST_CASE("Drivers: conditional wiring") {
    auto mm = make_mm();

    SECTION("First branch") {
        REQUIRE(run(mm, "Chain driver") == 2);
        REQUIRE(mm.at("Chain driver").inactive_settings().empty());
    }

    SECTION("Second branch forwards an input") {
        mm.change_input("Chain driver", "mode", std::string("double"));
        mm.change_input("Chain driver", "offset", 10);
        REQUIRE(run(mm, "Chain driver") == 12);
    }

    SECTION("Final branch") {
        mm.change_input("Chain driver", "mode", std::string("other"));
        REQUIRE(run(mm, "Chain driver") == 102);
    }

    SECTION("Changed inputs only forwarded by branches not taken") {
        mm.change_input("Chain driver", "offset", 10);
        REQUIRE(run(mm, "Chain driver") == 2);
        auto inactive = mm.at("Chain driver").inactive_settings();
        REQUIRE(inactive.size() == 1);
        REQUIRE(inactive[0] == "\"Chain driver\": \"offset\" has no effect "
                               "because no branch that forwards it was taken");
    }
}

TEST_CASE("Drivers: forwarding a submodule") {
    auto mm = make_mm();
    mm.copy_module("Leaf", "Ten");
    mm.change_input("Ten", "x", 10);
    mm.change_submod("Submod driver", "b", "Ten");
    REQUIRE(run(mm, "Submod driver") == 11);
    REQUIRE(run(mm, "Wrapper") == 2);
}

TEST_CASE("Drivers: user requests") {
    auto mm = make_mm();
    mm.change_input("X driver", "x", 5);

    SECTION("Override the developer's requests") {
        mm.at("X driver").call_graph_request().set_input("Leaf", "x", 20);
        REQUIRE(run(mm, "X driver") == 21);
    }

    SECTION("Set targets the driver does not") {
        mm.at("X driver").call_graph_request().set_input("Adder", "offset", 3);
        REQUIRE(run(mm, "X driver") == 9);
    }

    SECTION("Wrong type") {
        mm.at("X driver")
          .call_graph_request()
          .set_input("Adder", "offset", std::string("3"));
        REQUIRE_THROWS_AS(run(mm, "X driver"), std::invalid_argument);
    }
}

TEST_CASE("Drivers: memoization policy") {
    auto mm = make_mm();
    REQUIRE(run(mm, "Memo driver") == 2);
    REQUIRE(run(mm, "Memo driver") == 2);

    // "Leaf" memoizes, the other modules do not
    REQUIRE(Leaf::calls == 1);
    REQUIRE(OtherLeaf::calls == 2);

    // The ModuleManager's modules are unchanged
    REQUIRE(mm.at("Other leaf").is_memoizable());
}

TEST_CASE("Drivers: nested drivers") {
    auto mm = make_mm();
    // The outer driver sets the nested driver's "x", which it forwards
    REQUIRE(run(mm, "Outer driver") == 8);
    // The nested driver in the ModuleManager is unchanged
    REQUIRE(mm.at("X driver").inputs().at("x").value<int>() == 1);
    REQUIRE(run(mm, "X driver") == 2);
}
