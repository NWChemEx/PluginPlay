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

#include "export_fields.hpp"
#include <pluginplay/fields/module_input.hpp>
#include <pybind11/operators.h>

namespace pluginplay {

void export_module_input(py_module_reference m) {
    using python::PythonWrapper;

    py_class_type<ModuleInput>(m, "ModuleInput")
      .def(pybind11::init<>())
      .def("has_type", &ModuleInput::has_type)
      .def("has_value", &ModuleInput::has_value)
      .def("has_description", &ModuleInput::has_description)
      .def("is_optional", &ModuleInput::is_optional)
      .def("is_transparent", &ModuleInput::is_transparent)
      .def("ready", &ModuleInput::ready)
      .def("is_valid",
           [](ModuleInput& i, pybind11::object o) {
               return i.is_valid(any::make_any_field<PythonWrapper>(o));
           })
      .def("change",
           [](ModuleInput& i, pybind11::object o) {
               i.change(any::make_any_field<PythonWrapper>(o));
               return i;
           })
      .def("set_default",
           [](ModuleInput& i, pybind11::object o) {
               return i.set_default(any::make_any_field<PythonWrapper>(o));
           })
      .def("set_description", &ModuleInput::set_description)
      //.def("add_check")
      .def("make_optional", &ModuleInput::make_optional)
      .def("make_required", &ModuleInput::make_required)
      .def("make_opaque", &ModuleInput::make_opaque)
      .def("make_transparent", &ModuleInput::make_transparent)
      .def("__str__", &ModuleInput::str)
      .def(
        "value",
        [](ModuleInput& i) {
            return i.value<PythonWrapper>().template unwrap<pybind11::object>();
        })
      .def("description", &ModuleInput::description)
      .def("check_descriptions", &ModuleInput::check_descriptions)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace pluginplay
