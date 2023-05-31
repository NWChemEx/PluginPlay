/*
 * Copyright 2023 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "export_any.hpp"
#include <pluginplay/any/any_field.hpp>
#include <pybind11/operators.h>

namespace pluginplay::any {

void export_any_field(py_module_reference m) {
    py_class_type<AnyField>(m, "AnyField")
      .def(pybind11::init<>())
      .def("reset", &AnyField::reset)
      //.def("is_convertible") Not sure we need to expose this to Python
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def("has_value", &AnyField::has_value)
      .def("owns_value", &AnyField::owns_value);
}

} // namespace pluginplay::any
