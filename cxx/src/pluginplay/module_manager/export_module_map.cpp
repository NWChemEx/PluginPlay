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

#include "../module/py_module_base.hpp"
#include "export_module_manager.hpp"
#include <pluginplay/module_manager/module_map.hpp>

namespace pluginplay {

void export_module_map(py_module_reference m) {
    using key_type = ModuleMap::key_type;

    // Module is bound with the default (unique_ptr) holder, so Python can't
    // share ownership of a Module. Hence, only the key-based change_submod and
    // default_submod overloads are exposed, and at returns a reference.
    using change_by_key =
      void (ModuleMap::*)(const key_type&, const key_type&, const key_type&);
    using default_by_key =
      bool (ModuleMap::*)(const key_type&, const key_type&, const key_type&);

    py_class_type<ModuleMap>(m, "ModuleMap")
      .def(py::init<>())
      .def("count", &ModuleMap::count)
      .def("size", &ModuleMap::size)
      .def("__len__", &ModuleMap::size)
      .def("empty", &ModuleMap::empty)
      .def("keys", &ModuleMap::keys)
      .def(
        "at",
        [](ModuleMap& self, const key_type& key) -> Module& {
            return *self.at(key);
        },
        py::return_value_policy::reference_internal)
      .def(
        "add_module",
        [](ModuleMap& self, key_type key, std::shared_ptr<PyModuleBase> p) {
            self.add_module(std::move(key), p);
        },
        py::keep_alive<1, 3>())
      .def("change_submod",
           static_cast<change_by_key>(&ModuleMap::change_submod))
      .def("has_submod", &ModuleMap::has_submod)
      .def("default_submod",
           static_cast<default_by_key>(&ModuleMap::default_submod))
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace pluginplay
