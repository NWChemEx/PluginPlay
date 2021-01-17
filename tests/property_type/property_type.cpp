#include "../test_common.hpp"
#include "sde/property_type/property_type.hpp"
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
            using inputs_t      = decltype(inputs);
            using corr_inputs_t = sde::FieldTuple<sde::ModuleInput>;
            STATIC_REQUIRE(std::is_same_v<corr_inputs_t, inputs_t>);
        }

        SECTION("value of inputs") {
            sde::FieldTuple<sde::ModuleInput> corr_inputs;
            REQUIRE(corr_inputs == inputs);
        }
    }

    SECTION("results()") {
        auto results = pt::results();

        SECTION("type of results") {
            using results_t      = decltype(results);
            using corr_results_t = sde::FieldTuple<sde::ModuleResult>;
            STATIC_REQUIRE(std::is_same_v<corr_results_t, results_t>);
        }

        SECTION("value of results") {
            sde::FieldTuple<sde::ModuleResult> corr_results;
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
              sde::FieldTuple<sde::ModuleInput, double, int>;
            STATIC_REQUIRE(std::is_same_v<inputs_t, corr_inputs_t>);
        }

        SECTION("value of inputs") {
            sde::FieldTuple<sde::ModuleInput> corr_inputs;
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
              sde::FieldTuple<sde::ModuleInput, std::string, double, int>;
            STATIC_REQUIRE(std::is_same_v<inputs_t, corr_inputs_t>);
        }

        SECTION("value of inputs") {
            sde::FieldTuple<sde::ModuleInput> corr_inputs;
            auto corr = corr_inputs.add_field<std::string>("Option 3")
                          .add_field<double>("Option 2")
                          .add_field<int>("Option 1");
            REQUIRE(corr == inputs);
        }
    }
}