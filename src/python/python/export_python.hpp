#pragma once
#include "../export_pluginplay.hpp"

namespace pluginplay::python {

void export_python_wrapper(py_module_reference m);

inline void export_python(py_module_reference m) { export_python_wrapper(m); }

} // namespace pluginplay::python
