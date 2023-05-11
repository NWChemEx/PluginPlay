#include "export_any.hpp"
#include <pluginplay/any/any_field.hpp>
#include <pybind11/operators.h>

namespace pluginplay::any {

void export_any_field(pybind11::module_& m) {
    pybind11::class_<AnyField>(m, "AnyField")
      .def(pybind11::init<>())
      .def("reset", &AnyField::reset)
      //.def("is_convertible") Not sure we need to expose this to Python
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def("has_value", &AnyField::has_value)
      .def("owns_value", &AnyField::owns_value);
}

} // namespace pluginplay::any
