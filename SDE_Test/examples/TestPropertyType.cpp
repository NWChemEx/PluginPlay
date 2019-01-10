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
        REQUIRE(inputs["Dimension 1"].description() ==
                "The length of the 1st dimension");
        REQUIRE(inputs["Dimension 2"].description() ==
                "The length of the 2nd dimension");
    }

    SECTION("Wrap Inputs") {
        // Uncomment to check static assertion
        // pt.wrap_inputs(std::string{"Hi"}, double{3.14});
        auto inputs = pt.wrap_inputs(pt.inputs(), double{1.23}, double{4.56});
        SECTION("Manually unwrap") {
            REQUIRE(inputs.size() == 2);
            REQUIRE(inputs["Dimension 1"].value<double>() == 1.23);
            REQUIRE(inputs["Dimension 2"].value<double>() == 4.56);
        }
        SECTION("Auto unwrap") {
            auto[dim1, dim2] = Area::unwrap_inputs(inputs);
            REQUIRE(dim1 == 1.23);
            REQUIRE(dim2 == 4.56);
        }
    }

    SECTION("Outputs") {
        auto outputs = pt.outputs();
        REQUIRE(outputs.size() == 1);
        REQUIRE(outputs["Area"].description() == "The area of the shape");
    }
    SECTION("Wrap Outputs") {
        auto outputs = Area::wrap_outputs(pt.outputs(), double{1.23});
        SECTION("Manually unwrap") {
            REQUIRE(outputs.at("Area").value<double>() == 1.23);
        }

        SECTION("Unwrap Auto Wrapped Outputs") {
            auto[area] = Area::unwrap_outputs(outputs);
            REQUIRE(area == 1.23);
        }
    }
}

TEST_CASE("PrismVolume Property Type") {
    PrismVolume pt;
    SECTION("Inputs") {
        auto inputs = pt.inputs();
        REQUIRE(inputs.size() == 1);
        REQUIRE(inputs["Dimensions"].description() ==
                "The length of each dimension");
    }

    SECTION("Wrap Inputs") {
        using v_double = std::vector<double>;
        v_double dims{1.23, 4.56, 7.89};
        auto inputs = pt.wrap_inputs(pt.inputs(), dims);
        REQUIRE(inputs.size() == 1);
        SECTION("Manually unwrap") {
            const auto& pdims = inputs["Dimensions"].value<const v_double&>();
            REQUIRE(&pdims == &dims);
        }
        SECTION("Auto unwrap Inputs") {
            const auto & [pdims] = PrismVolume::unwrap_inputs(inputs);
            REQUIRE(&pdims == &dims);
        }
    }

    SECTION("Outputs") {
        auto outputs = pt.outputs();
        REQUIRE(outputs.size() == 2);
        REQUIRE(outputs["Base area"].description() == "The area of the base");
        REQUIRE(outputs["Volume"].description() == "The volume of the prism");
    }

    SECTION("Wrap Outputs") {
        double v1    = 1.23;
        double v2    = 4.56;
        auto results = pt.outputs();
        results      = PrismVolume::wrap_outputs(results, v1, v2);
        SECTION("Manually unwrap") {
            REQUIRE(results.at("Base area").value<double>() == v1);
            REQUIRE(results.at("Volume").value<double>() == v2);
        }
        SECTION("Auto unwrap Outputs") {
            auto[area, volume] = pt.unwrap_outputs(results);
            REQUIRE(area == 1.23);
            REQUIRE(volume == 4.56);
        }
    }
}
