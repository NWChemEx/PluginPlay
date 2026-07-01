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

#include "export_python.hpp"
#include <pluginplay/python/python_wrapper.hpp>

namespace pluginplay::python {

void export_python_wrapper(py_module_reference m) {
    py_class_type<PythonWrapper>(m, "PythonWrapper")
      .def(py::init<py::object>())
      .def("has_value", &PythonWrapper::has_value)
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace pluginplay::python
