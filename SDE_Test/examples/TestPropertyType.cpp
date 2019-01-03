#include "examples/TestPropertyType.hpp"
#include <catch2/catch.hpp>

/* This test focuses on the PropertyType class. The PropertyType class can not
 * be directly tested as it is an abstract class relying on compile-time
 * polymorphism. For this reason we test two example property types: "Area" and
 * "Volume". The code for these classes is contained within the
 * "examples/TestPropertyType.hpp" header file. These property types will be
 * used later for other examples tests.
 *
 */

TEST_CASE("Area Property Type") {
    Area pt;
    SECTION("Inputs") {
        auto inputs = pt.inputs();
        REQUIRE(inputs.size() == 2);
        REQUIRE(inputs[0].first == "Dimension 1");
        REQUIRE(inputs[0].second.desc == "The length of the first dimension");
        REQUIRE(inputs[1].first == "Dimension 2");
        REQUIRE(inputs[1].second.desc == "The length of the second dimension");
    }

    SECTION("Wrap Inputs") {
        // Uncomment to check static assertion
        // pt.wrap_inputs(std::string{"Hi"}, double{3.14});
        auto inputs = pt.wrap_inputs(double{1.23}, double{4.56});
        REQUIRE(inputs.size() == 2);
        REQUIRE(inputs[0].second.value<double>() == 1.23);
        REQUIRE(inputs[1].second.value<double>() == 4.56);
    }

    SECTION("Outputs") {
        auto outputs = pt.outputs();
        REQUIRE(outputs.size() == 1);
        REQUIRE(outputs[0].first == "Area");
        REQUIRE(outputs[0].second.desc == "The area of the shape");
    }

    SECTION("Unwrap Outputs") {
        auto outputs = pt.outputs();
        outputs[0].second.change(double{1.23});
        std::map<std::string, SDE::ModuleOutput> results;
        results.emplace(outputs[0]);
        double area = pt.unwrap_outputs(results);
        REQUIRE(area == 1.23);
    }
}

TEST_CASE("PrismVolume Property Type") {
    PrismVolume pt;
    SECTION("Inputs") {
        auto inputs = pt.inputs();
        REQUIRE(inputs.size() == 1);
        REQUIRE(inputs[0].first == "Dimensions");
        REQUIRE(inputs[0].second.desc == "The length of each dimension");
    }

    SECTION("Wrap Inputs") {
        using v_double = std::vector<double>;
        v_double dims{1.23, 4.56, 7.89};
        auto inputs = pt.wrap_inputs(dims);
        REQUIRE(inputs.size() == 1);
        const auto& pdims = inputs[0].second.value<const v_double&>();
        REQUIRE(&pdims == &dims);
    }

    SECTION("Outputs") {
        auto outputs = pt.outputs();
        REQUIRE(outputs.size() == 2);
        REQUIRE(outputs[0].first == "Base area");
        REQUIRE(outputs[0].second.desc == "The area of the base");
        REQUIRE(outputs[1].first == "Volume");
        REQUIRE(outputs[1].second.desc == "The volume of the prism");
    }

    SECTION("Unwrap Outputs") {
        auto outputs = pt.outputs();
        outputs[0].second.change(double{1.23});
        outputs[1].second.change(double{4.56});
        std::map<std::string, SDE::ModuleOutput> results;
        results.emplace(outputs[0]);
        results.emplace(outputs[1]);
        auto[area, volume] = pt.unwrap_outputs(results);
        REQUIRE(area == 1.23);
        REQUIRE(volume == 4.56);
    }
}
