#pragma once
#include <pybind11/pybind11.h>

namespace test_pluginplay {

void test_module(pybind11::module_& m);
void test_module_base(pybind11::module_& m);
void test_submodule_request(pybind11::module_& m);

} // namespace test_pluginplay
