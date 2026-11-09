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
#include "../driver/driver_test_modules.hpp"

using namespace pluginplay;
using kind_type = WiringRequest::kind_type;

namespace {

// A driver whose constructor exercises the developer's API
struct TestDriver : DriverBase {
    TestDriver() : DriverBase(this) {
        add_input<int>("x")
          .set_description("An input")
          .set_default(1)
          .set_target("Module A", "x")
          .set_required_target("Module B", "x");
        add_input<bool>("flag").set_default(false);
        add_submodule<testing::drivers::Value>("helper")
          .set_description("A submodule")
          .set_default("A helper")
          .set_target("Module A", "helper");

        when("flag", true)
          .set_input("Module C", "y", 2)
          .forward_submod("Module C", "helper", "helper")
          .otherwise("x", 3)
          .required()
          .set_submod("Module C", "helper", "Another helper")
          .otherwise()
          .forward_input("Module C", "y", "x");
        wire_memoization(false, {"Module A"});
    }
    result_map_type run_(input_map_type, submodule_map_type) const override {
        return results();
    }
};

} // namespace

TEST_CASE("WiringRequest") {
    auto set_input =
      WiringRequest::make_set_input("A", "x", any::make_any_field<int>(1));
    auto set_submod = WiringRequest::make_set_submod("A", "s", "B");
    auto fwd_input  = WiringRequest::make_forward_input("A", "x", "y");
    auto fwd_submod = WiringRequest::make_forward_submod("A", "s", "t");

    SECTION("Accessors") {
        REQUIRE(set_input.kind() == kind_type::set_input);
        REQUIRE(set_input.sets_input());
        REQUIRE_FALSE(set_input.is_forward());
        REQUIRE(set_input.module_key() == QualifiedKey("A"));
        REQUIRE(set_input.target_key() == "x");
        REQUIRE(set_input.value() == any::make_any_field<int>(1));
        REQUIRE_THROWS_AS(set_input.submod_key(), std::runtime_error);
        REQUIRE_THROWS_AS(set_input.source_key(), std::runtime_error);

        REQUIRE(set_submod.kind() == kind_type::set_submod);
        REQUIRE_FALSE(set_submod.sets_input());
        REQUIRE(set_submod.submod_key() == QualifiedKey("B"));
        REQUIRE_THROWS_AS(set_submod.value(), std::runtime_error);

        REQUIRE(fwd_input.is_forward());
        REQUIRE(fwd_input.sets_input());
        REQUIRE(fwd_input.source_key() == "y");

        REQUIRE(fwd_submod.is_forward());
        REQUIRE_FALSE(fwd_submod.sets_input());
        REQUIRE(fwd_submod.source_key() == "t");
    }

    SECTION("required") {
        REQUIRE_FALSE(set_input.is_required());
        REQUIRE(set_input.required().is_required());
        REQUIRE_FALSE(set_input.required(false).is_required());
    }

    SECTION("to_string") {
        REQUIRE(set_input.to_string() == "setting \"A\"'s input \"x\" to 1");
        REQUIRE(set_submod.to_string() ==
                "setting \"A\"'s submodule \"s\" to \"B\"");
        REQUIRE(fwd_input.to_string() ==
                "forwarding input \"y\" to \"A\"'s input \"x\"");
        REQUIRE(fwd_submod.to_string() ==
                "forwarding submodule \"t\" to \"A\"'s submodule \"s\"");
    }

    SECTION("Comparisons") {
        REQUIRE(set_input == WiringRequest::make_set_input(
                               "A", "X", any::make_any_field<int>(1)));
        REQUIRE(set_input != WiringRequest::make_set_input(
                               "A", "x", any::make_any_field<int>(2)));
        REQUIRE(fwd_input != fwd_submod);
        auto copy = set_input;
        REQUIRE(copy.required() != set_input);
    }
}

TEST_CASE("CallGraphRequest") {
    CallGraphRequest request;
    REQUIRE(request.empty());

    SECTION("The user's API") {
        request.set_input("A", "x", "a string");
        request.set_submod({"plugin", "A"}, "s", "B").required();
        REQUIRE_FALSE(request.empty());
        REQUIRE(request.requests().size() == 2);
        // C-strings are stored as std::string
        REQUIRE(request.requests()[0].value() ==
                any::make_any_field<std::string>("a string"));
        REQUIRE(request.requests()[1].module_key() ==
                QualifiedKey("plugin", "A"));
        REQUIRE(request.requests()[1].is_required());
        // The user's requests may set the same target twice
        REQUIRE_NOTHROW(request.set_input("A", "x", 2));
    }

    SECTION("Copies and comparisons") {
        TestDriver driver;
        const auto& dev = driver.call_graph_request();
        CallGraphRequest copy(dev);
        REQUIRE(copy == dev);
        REQUIRE(copy != request);

        CallGraphRequest moved(std::move(copy));
        REQUIRE(moved == dev);
        request = moved;
        REQUIRE(request == dev);
    }
}

TEST_CASE("DriverBase") {
    TestDriver driver;
    const auto& request = driver.call_graph_request();

    SECTION("Inputs and submodules") {
        const auto& x = driver.inputs().at("x");
        REQUIRE(x.description() == "An input");
        REQUIRE(x.value<int>() == 1);
        REQUIRE(driver.submods().at("helper").description() == "A submodule");
    }

    SECTION("set_target") {
        const auto& requests = request.requests();
        REQUIRE(requests.size() == 3);
        REQUIRE(requests[0] ==
                WiringRequest::make_forward_input("Module A", "x", "x"));
        REQUIRE(
          requests[1] ==
          WiringRequest::make_forward_input("Module B", "x", "x").required());
        REQUIRE(requests[2] == WiringRequest::make_forward_submod(
                                 "Module A", "helper", "helper"));
        REQUIRE(request.submod_defaults().at("helper") ==
                QualifiedKey("A helper"));
    }

    SECTION("Chains") {
        const auto& chains = request.conditional_wiring();
        REQUIRE(chains.size() == 1);
        const auto& branches = chains[0].branches();
        REQUIRE(branches.size() == 3);

        REQUIRE(branches[0].input_key() == "flag");
        REQUIRE(branches[0].value() == any::make_any_field<bool>(true));
        REQUIRE(branches[0].condition_string() == "\"flag\" is 1");
        REQUIRE(branches[0].requests().size() == 2);
        REQUIRE_FALSE(branches[0].is_required());

        REQUIRE(branches[1].input_key() == "x");
        REQUIRE(branches[1].is_required());
        REQUIRE(branches[1].requests()[0].is_required());

        REQUIRE_FALSE(branches[2].has_condition());
        REQUIRE_THROWS_AS(branches[2].value(), std::runtime_error);
        REQUIRE(branches[2].condition_string() == "no other condition holds");
    }

    SECTION("Memoization policy") {
        const auto& policy = request.memoization_policy();
        REQUIRE(policy.has_value());
        REQUIRE_FALSE(policy->on);
        REQUIRE(policy->except == std::vector<QualifiedKey>{"Module A"});
    }

    SECTION("Errors") {
        struct UnknownInput : DriverBase {
            UnknownInput() : DriverBase(this) {
                add_input<int>("x");
                when("y", 1);
            }
            result_map_type run_(input_map_type,
                                 submodule_map_type) const override {
                return results();
            }
        };
        struct UnknownForward : DriverBase {
            UnknownForward() : DriverBase(this) {
                add_input<int>("x");
                when("x", 1).forward_input("A", "x", "y");
            }
            result_map_type run_(input_map_type,
                                 submodule_map_type) const override {
                return results();
            }
        };
        struct BranchAfterFinal : DriverBase {
            BranchAfterFinal() : DriverBase(this) {
                add_input<int>("x");
                when("x", 1).otherwise().otherwise("x", 2);
            }
            result_map_type run_(input_map_type,
                                 submodule_map_type) const override {
                return results();
            }
        };
        // Inputs of a property type are passed on each call, after the
        // requests are resolved, so they can not be conditions or forwarded
        using testing::drivers::ValueOfX;
        struct PTCondition : DriverBase {
            PTCondition() : DriverBase(this) {
                satisfies_property_type<ValueOfX>();
                when("x", 1);
            }
            result_map_type run_(input_map_type,
                                 submodule_map_type) const override {
                return results();
            }
        };
        struct PTForward : DriverBase {
            PTForward() : DriverBase(this) {
                satisfies_property_type<ValueOfX>();
                add_input<int>("y");
                when("y", 1).forward_input("A", "x", "x");
            }
            result_map_type run_(input_map_type,
                                 submodule_map_type) const override {
                return results();
            }
        };
        struct PTTarget : DriverBase {
            PTTarget() : DriverBase(this) {
                satisfies_property_type<ValueOfX>();
                driver_input("x").set_target("A", "x");
            }
            result_map_type run_(input_map_type,
                                 submodule_map_type) const override {
                return results();
            }
        };
        REQUIRE_THROWS_AS(UnknownInput{}, std::out_of_range);
        REQUIRE_THROWS_AS(UnknownForward{}, std::out_of_range);
        REQUIRE_THROWS_AS(BranchAfterFinal{}, std::logic_error);
        REQUIRE_THROWS_AS(PTCondition{}, std::invalid_argument);
        REQUIRE_THROWS_AS(PTForward{}, std::invalid_argument);
        REQUIRE_THROWS_AS(PTTarget{}, std::invalid_argument);
    }
}
