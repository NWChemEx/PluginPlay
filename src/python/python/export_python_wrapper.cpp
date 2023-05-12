#include "export_python.hpp"
#include <pluginplay/python/python_wrapper.hpp>
#include <pybind11/operators.h>

namespace pluginplay::python {

void export_python_wrapper(py_module_reference m) {
    py_class_type<PythonWrapper>(m, "PythonWrapper")
      .def(pybind11::init<pybind11::object>())
      .def("has_value", &PythonWrapper::has_value)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace pluginplay::python
