#include "export_fields.hpp"
#include <pluginplay/fields/module_result.hpp>
#include <pybind11/operators.h>

namespace pluginplay {

void export_module_result(py_module_reference m) {
    using python::PythonWrapper;

    py_class_type<ModuleResult>(m, "ModuleResult")
      .def(pybind11::init<>())
      .def("has_type", &ModuleResult::has_type)
      .def("has_value", &ModuleResult::has_value)
      .def("has_description", &ModuleResult::has_description)
      .def("change",
           [](ModuleResult& r, pybind11::object o) {
               return r.change(any::make_any_field<PythonWrapper>(o));
           })
      .def("set_description", &ModuleResult::set_description)
      .def(
        "value",
        [](ModuleResult& r) {
            return r.value<PythonWrapper>().template unwrap<pybind11::object>();
        })
      .def("description", &ModuleResult::description)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace pluginplay
