#include "examples/TestModuleBase.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Rectangle Class") {
    // There's only a default ctor
    Rectangle r;
    SECTION("Inputs") {
        auto inputs = r.inputs();
        REQUIRE(inputs.at("Dimension1").description() == "The height of the"
                                                         " rectangle");
        REQUIRE(inputs.at("Dimension2").description() == "The width of the "
                                                         "rectangle");
        REQUIRE(inputs.at("Name").description() == "The name of the "
                                                   "rectangle");
    }
    SECTION("Outputs") {
        auto outputs = r.outputs();
        REQUIRE(outputs.at("Area").description() == "The area of the shape");
        REQUIRE(outputs.at("Picture").description() == "An ASCII picture of "
                                                       "the rectangle");
    }
}
