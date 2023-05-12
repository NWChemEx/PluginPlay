#pragma once
#include <pybind11/pybind11.h>

namespace test_pluginplay {

void test_module_input(pybind11::module_& m);
void test_module_result(pybind11::module_& m);

inline void test_fields(pybind11::module_& m) {
    test_module_input(m);
    test_module_result(m);
}

} // namespace test_pluginplay
