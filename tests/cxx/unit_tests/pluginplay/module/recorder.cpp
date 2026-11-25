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
#include "../test_common.hpp"
#include <memory>
#include <pluginplay/module/lambda_module.hpp>
#include <pluginplay/module/recorder.hpp>
#include <pluginplay/module_manager/module_manager.hpp>
#include <set>
#include <vector>

using pt = testing::OptionalInput;

TEST_CASE("Recorder") {
    pluginplay::ModuleManager mm;

    // The wrapped module doubles its input, and keeps the inputs it is called
    // with
    std::vector<int> wrapped_inputs;
    auto wrapped = pluginplay::make_lambda<pt>([&](int i) {
        wrapped_inputs.push_back(i);
        return 2 * i;
    });

    auto recorder = std::make_shared<pluginplay::Recorder>(pt{});
    mm.add_module("Recorder", recorder);
    mm.at("Recorder").change_submod("Wrapped", wrapped);

    SECTION("Satisfies the property type") {
        const auto& mod = mm.at("Recorder");
        REQUIRE(mod.property_types() ==
                std::set<pluginplay::type::rtti>{typeid(pt)});
        REQUIRE(mod.inputs().count("Option 1"));
        REQUIRE(mod.results().count("Result 1"));
        REQUIRE(mod.submods().at("Wrapped").ready());
    }

    SECTION("No records before it is run") {
        REQUIRE(recorder->records().empty());
    }

    SECTION("Forwards the inputs and returns the results") {
        REQUIRE(mm.at("Recorder").run_as<pt>(3) == 6);
        REQUIRE(wrapped_inputs == std::vector<int>{3});
    }

    SECTION("Records the results of each run, in order") {
        mm.at("Recorder").run_as<pt>(3);
        mm.at("Recorder").run_as<pt>(4);

        const auto& records = recorder->records();
        REQUIRE(records.size() == 2);
        REQUIRE(std::get<0>(pt::unwrap_results(records[0])) == 6);
        REQUIRE(std::get<0>(pt::unwrap_results(records[1])) == 8);
    }

    SECTION("clear") {
        mm.at("Recorder").run_as<pt>(3);
        recorder->clear();
        REQUIRE(recorder->records().empty());
    }

    SECTION("Is not shareable") { REQUIRE_FALSE(recorder->is_shareable()); }

    SECTION("Several Recorders in one ModuleManager") {
        // A second Recorder, of another property type
        using other_pt = testing::OneOut;
        auto other     = std::make_shared<pluginplay::Recorder>(other_pt{});
        mm.add_module("Other Recorder", other);
        mm.at("Other Recorder")
          .change_submod("Wrapped",
                         pluginplay::make_lambda<other_pt>([]() { return 7; }));

        REQUIRE(mm.at("Recorder").run_as<pt>(3) == 6);
        REQUIRE(mm.at("Other Recorder").run_as<other_pt>() == 7);

        // Each keeps its own records
        REQUIRE(recorder->records().size() == 1);
        REQUIRE(std::get<0>(pt::unwrap_results(recorder->records()[0])) == 6);
        REQUIRE(other->records().size() == 1);
        REQUIRE(std::get<0>(other_pt::unwrap_results(other->records()[0])) ==
                7);
    }

    SECTION("Memoized runs are not recorded") {
        mm.at("Recorder").run_as<pt>(3);
        mm.at("Recorder").run_as<pt>(3);
        REQUIRE(recorder->records().size() == 1);

        mm.at("Recorder").turn_off_memoization();
        mm.at("Recorder").run_as<pt>(3);
        REQUIRE(recorder->records().size() == 2);
    }
}
