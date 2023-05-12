#pragma once
#include "../export_pluginplay.hpp"
#include <pluginplay/any/any.hpp>
#include <pluginplay/python/python_wrapper.hpp>

namespace pluginplay::any {

void export_any_field(py_module_reference m);

inline void export_any(py_module_reference m) {
    using py_object_type = python::PythonWrapper::py_object_type;
    auto m_any           = m.def_submodule("any");
    export_any_field(m_any);
    m.def("make_any_field", [](py_object_type o) {
        return make_any_field<python::PythonWrapper>(o);
    });
    m.def("any_cast",
          [](AnyField& da_any) { return any_cast<py_object_type>(da_any); });
}

} // namespace pluginplay::any
