#pragma once

/** @file Pythonization.hpp File for factoring out ifdefs related to Python
 *        bindings.
 *
 *  If Python bindings are enabled this file injects some types from Pybind11
 *  into the SDE namespace.  If bindings are not enabled, those same types are
 *  defined, but set to nullptrs.  Attempting to call any code that relies on
 *  Pybind11, with bindings disabled will cause a runtime_error.
 *
 */

#ifdef USING_pybind11
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace SDE {

/// The type of the C-side type of the opaque Python base class
using pyobject = pybind11::object;

/// Function for converting a C++ object into a Python object
template<typename T>
pyobject pycast(T&& value) {
    return pybind11::cast(std::forward<T>(value));
}

/// Function
template<typename T>
T castpy(pyobject& obj) {
    return obj.cast<T>();
}

} // namespace SDE
#else

namespace SDE {

using pyobject = decltype(nullptr);

template<typename T>
pyobject pycast(T&&) {
    throw std::runtime_error("Python Bindings were not enabled!");
}

template<typename T>
T castpy(pyobject& obj) {
    throw std::runtime_error("Python Bindings were not enabled!");
}

} // namespace SDE
#endif
