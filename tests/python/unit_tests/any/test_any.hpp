#pragma once
#include <pybind11/pybind11.h>

namespace test_pluginplay {

void test_any_field(pybind11::module_& m);

inline void test_any(pybind11::module_& m) { test_any_field(m); }

} // namespace test_pluginplay
