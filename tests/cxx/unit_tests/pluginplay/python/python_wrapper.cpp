#include <catch2/catch.hpp>
#include <pluginplay/python/python_wrapper.hpp>

using namespace pluginplay::python;

#ifdef BUILD_PYBIND11

TEST_CASE("PythonWrapper  -- BUILD_PYBIND11 is defined") {
    pybind11::object null_obj;
    PythonWrapper holds_null(null_obj);

    SECTION("CTors") {
        SECTION("value ctor") {
            REQUIRE_FALSE(holds_null.has_value());
            // REQUIRE(holds_value.has_value());
        }
    }

    SECTION("has_value") {
        REQUIRE_FALSE(holds_null.has_value());
        // REQUIRE(holds_value.has_value());
    }
}

#else

TEST_CASE("PythonWrapper -- BUILD_PYBIND11 not defined") {
    REQUIRE_THROWS_AS(PythonWrapper(), std::runtime_error);
}

#endif
