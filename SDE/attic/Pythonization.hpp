#pragma once
#include <SDE/detail_/Memoization.hpp>

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

/// Function for converting a Python object into a C++ object
template<typename T>
T castpy(pyobject& obj) {
    return obj.cast<T>();
}

namespace detail_ {
/// Implements guts of function for converting a C++ object into a Python object
template<typename T>
struct PyCaster {
    static pyobject cast(T value) { return pybind11::cast(value); }
};

/// Specialization for when it's already a python object
template<>
struct PyCaster<pybind11::object> {
    static pyobject cast(pybind11::object& value) { return value; }
};
} // namespace detail_

/// Function used to convert a C++ object to a Python object
template<typename T>
pyobject pycast(T value) {
    return detail_::PyCaster<T>::cast(value);
}

} // namespace SDE

namespace pybind11 {
void inline hash_object(const pybind11::object& cls, SDE::Hasher& h) {
    auto hash_fxn = cls.attr("__hash__");
    auto hash     = hash_fxn();
    h(hash.cast<std::string>());
}
} // namespace pybind11
#else

namespace SDE {

using pyobject = decltype(nullptr);

template<typename T>
T castpy(pyobject& obj) {
    throw std::runtime_error("Python Bindings were not enabled!");
}

template<typename T>
pyobject pycast(const T& value) {
    throw std::runtime_error("Python Bindings were not enabled!");
}

inline void hash_object(const pyobject& cls, SDE::Hasher& h) { h(cls); }

} // namespace SDE
#endif
