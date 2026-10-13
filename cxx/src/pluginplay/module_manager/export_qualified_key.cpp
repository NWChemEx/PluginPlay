/*
 * Copyright 2026 NWChemEx-Project
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

#include "export_module_manager.hpp"
#include <pluginplay/module_manager/qualified_key.hpp>
#include <stdexcept>

namespace pluginplay {

void export_qualified_key(py_module_reference m) {
    using name_type = QualifiedKey::name_type;
    using key_type  = QualifiedKey::key_type;

    py_class_type<QualifiedKey>(m, "QualifiedKey")
      .def(py::init<key_type>())
      .def(py::init<name_type, key_type>())
      .def(py::init([](const py::tuple& t) {
          if(t.size() != 2)
              throw std::invalid_argument(
                "A QualifiedKey tuple must be (plugin name, module key)");
          return QualifiedKey(t[0].cast<name_type>(), t[1].cast<key_type>());
      }))
      .def("is_qualified", &QualifiedKey::is_qualified)
      .def("plugin", &QualifiedKey::plugin)
      .def("key", &QualifiedKey::key)
      .def("to_string", &QualifiedKey::to_string)
      .def("__str__", &QualifiedKey::to_string)
      .def("__repr__",
           [](const QualifiedKey& self) {
               return "QualifiedKey('" + self.to_string() + "')";
           })
      .def(py::self == py::self)
      .def(py::self != py::self);

    // Lets Python pass "key" or ("plugin", "key") wherever a QualifiedKey is
    // expected
    py::implicitly_convertible<key_type, QualifiedKey>();
    py::implicitly_convertible<py::tuple, QualifiedKey>();
}

} // namespace pluginplay
