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

#include "../test_common.hpp"
#include "pluginplay/property_type/property_type.hpp"
#include <catch2/catch.hpp>
#include <type_traits>

TEST_CASE("PropertyType<T> T = NullPT") {
    using pt = testing::NullPT;

    SECTION("base_property_types") {
        using base_pt = typename pt::bases_t;
        STATIC_REQUIRE(std::is_same_v<base_pt, std::tuple<>>);
    }

    SECTION("inputs()") {
        auto inputs = pt::inputs();

        SECTION("type of inputs") {
            using inputs_t = decltype(inputs);
            using corr_inputs_t =
              pluginplay::FieldTuple<pluginplay::ModuleInput>;
            STATIC_REQUIRE(std::is_same_v<corr_inputs_t, inputs_t>);
        }

        SECTION("value of inputs") {
            pluginplay::FieldTuple<pluginplay::ModuleInput> corr_inputs;
            REQUIRE(corr_inputs == inputs);
        }
    }

    SECTION("results()") {
        auto results = pt::results();

        SECTION("type of results") {
            using results_t = decltype(results);
            using corr_results_t =
              pluginplay::FieldTuple<pluginplay::ModuleResult>;
            STATIC_REQUIRE(std::is_same_v<corr_results_t, results_t>);
        }

        SECTION("value of results") {
            pluginplay::FieldTuple<pluginplay::ModuleResult> corr_results;
            REQUIRE(corr_results == results);
        }
    }
}

TEST_CASE("PropertyType<T> T = TwoIn") {
    using pt = testing::TwoIn;

    SECTION("base_property_types") {
        using base_pt = typename pt::bases_t;
        STATIC_REQUIRE(std::is_same_v<base_pt, std::tuple<testing::OneIn>>);
    }

    SECTION("inputs()") {
        auto inputs = pt::inputs();

        SECTION("type of inputs") {
            using inputs_t = decltype(inputs);
            using corr_inputs_t =
              pluginplay::FieldTuple<pluginplay::ModuleInput, double, int>;
            STATIC_REQUIRE(std::is_same_v<inputs_t, corr_inputs_t>);
        }

        SECTION("value of inputs") {
            pluginplay::FieldTuple<pluginplay::ModuleInput> corr_inputs;
            auto corr = corr_inputs.add_field<double>("Option 2")
                          .add_field<int>("Option 1");
            REQUIRE(corr == inputs);
        }
    }
}

TEST_CASE("PropertyType<T> T = ThreeIn") {
    using pt = testing::ThreeIn;

    SECTION("base_property_types") {
        using base_pt = typename pt::bases_t;
        using corr    = std::tuple<testing::TwoIn, testing::OneIn>;
        STATIC_REQUIRE(std::is_same_v<base_pt, corr>);
    }

    SECTION("inputs()") {
        auto inputs = pt::inputs();

        SECTION("type of inputs") {
            using inputs_t = decltype(inputs);
            using corr_inputs_t =
              pluginplay::FieldTuple<pluginplay::ModuleInput, std::string,
                                     double, int>;
            STATIC_REQUIRE(std::is_same_v<inputs_t, corr_inputs_t>);
        }

        SECTION("value of inputs") {
            pluginplay::FieldTuple<pluginplay::ModuleInput> corr_inputs;
            auto corr = corr_inputs.add_field<std::string>("Option 3")
                          .add_field<double>("Option 2")
                          .add_field<int>("Option 1");
            REQUIRE(corr == inputs);
        }
    }
}
