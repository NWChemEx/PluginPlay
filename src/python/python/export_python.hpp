#pragma once
#include <pybind11/pybind11.h>

namespace pluginplay::python {

void export_python_wrapper(pybind11::module_& m);

inline void export_python(pybind11::module_& m) { export_python_wrapper(m); }

} // namespace pluginplay::python
