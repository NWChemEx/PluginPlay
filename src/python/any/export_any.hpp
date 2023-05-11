#pragma once
#include <pluginplay/any/any.hpp>
#include <pluginplay/python/python_wrapper.hpp>
#include <pybind11/pybind11.h>

namespace pluginplay::any {

void export_any_field(pybind11::module_& m);

inline void export_any(pybind11::module_& m) {
    auto m_any = m.def_submodule("any");
    export_any_field(m_any);
    m.def("make_any_field", [](pybind11::object o) {
        return make_any_field<python::PythonWrapper>(o);
    });
    m.def("any_cast",
          [](AnyField& da_any) { return any_cast<pybind11::object>(da_any); });
}

} // namespace pluginplay::any
