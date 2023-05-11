#include <catch2/catch.hpp>
#include <pluginplay/python/detail_/python_wrapper_pimpl.hpp>
#include <pybind11/stl.h>

using namespace pluginplay::python::detail_;

#ifdef ENABLE_PYBIND11
TEST_CASE("PythonWrapperPIMPL(PYBIND11_ENABLED)") {
    using object_type = typename PythonWrapperPIMPL::object_type;
    std::vector<double> cxx_corr{1.23, 2.34, 3.45};
    pybind11::list l = pybind11::cast(cxx_corr);
    auto da_any      = std::make_any<object_type>(l);

    PythonWrapperPIMPL no_value(std::make_any<object_type>());
    PythonWrapperPIMPL value(std::move(da_any));

    SECTION("CTor") {
        REQUIRE_FALSE(no_value.has_value());
        REQUIRE(value.has_value());
    }
}
#endif
