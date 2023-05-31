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
               r.change(any::make_any_field<PythonWrapper>(o));
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
