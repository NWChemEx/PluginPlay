#pragma once
#include <pybind11/pybind11.h>

namespace pluginplay {

/// Type of a C++ handle to a Python module
using py_module_type = pybind11::module_;

/// Type of a reference to an object of type py_module_type
using py_module_reference = py_module_type&;

/// Type of Python object binding for a C++ class of type @p T
template<typename T>
using py_class_type = pybind11::class_<T>;

} // namespace pluginplay
