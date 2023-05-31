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
#include <pluginplay/python/py_type_info.hpp>
#include <pybind11/operators.h>

namespace pluginplay::python {

void export_py_type_info(py_module_reference m) {
    py_class_type<PyTypeInfo>(m, "PyTypeInfo")
      .def(pybind11::init<>())
      .def("has_value", &PyTypeInfo::has_value)
      .def("__str__", &PyTypeInfo::name)
      .def("__hash__",
           [](PyTypeInfo& self) {
               return self.has_value() ? self.value().hash_code() :
                                         std::size_t{};
           })
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace pluginplay::python
