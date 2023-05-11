#include <catch2/catch.hpp>
#include <pluginplay/python/python_wrapper.hpp>
#include <pybind11/pybind11.h>

using namespace pluginplay::python;

TEST_CASE("PythonWrapper") {
    PythonWrapper defaulted;

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE_FALSE(defaulted.has_value()); }
    }

    SECTION("comparisons") {
        PythonWrapper other_defaulted;
        REQUIRE(other_defaulted == defaulted);
        REQUIRE_FALSE(other_defaulted != defaulted);
    }
}
