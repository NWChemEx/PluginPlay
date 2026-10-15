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
#include "module_map_view.hpp"
#include <pluginplay/any/any.hpp>
#include <pluginplay/module_manager/module_map.hpp>
#include <pluginplay/python/python_wrapper.hpp>

namespace pluginplay {

void export_module_map(py_module_reference m) {
    using key_type = ModuleMap::key_type;

    // Only the key-based change_submod and default_submod overloads are
    // exposed. at returns the module with shared ownership.
    using change_by_key =
      void (ModuleMap::*)(const key_type&, const key_type&, const key_type&);
    using default_by_key =
      bool (ModuleMap::*)(const key_type&, const key_type&, const key_type&);

    py_class_type<ModuleMapView>(m, "ModuleMapView")
      .def("count", &ModuleMapView::count)
      .def("__contains__", &ModuleMapView::count)
      .def("size", &ModuleMapView::size)
      .def("__len__", &ModuleMapView::size)
      .def("empty", &ModuleMapView::empty)
      .def("keys", &ModuleMapView::keys)
      .def("at", &ModuleMapView::at)
      .def("has_submod", &ModuleMapView::has_submod)
      .def(py::self == py::self)
      .def(py::self != py::self);

    py_class_type<ModuleMap>(m, "ModuleMap")
      .def(py::init<>())
      .def("count", &ModuleMap::count)
      .def("size", &ModuleMap::size)
      .def("__len__", &ModuleMap::size)
      .def("empty", &ModuleMap::empty)
      .def("keys", &ModuleMap::keys)
      .def("at", &ModuleMap::at)
      .def(
        "add_module",
        [](ModuleMap& self, key_type key, std::shared_ptr<PyModuleBase> p) {
            self.add_module(std::move(key), p);
        },
        py::keep_alive<1, 3>())
      .def("copy_module", &ModuleMap::copy_module)
      .def("erase", &ModuleMap::erase)
      .def("change_input",
           [](ModuleMap& self, const key_type& key, const key_type& option,
              py::object value) {
               using python::PythonWrapper;
               self.change_input(key, option,
                                 any::make_any_field<PythonWrapper>(value));
           })
      .def("change_submod",
           static_cast<change_by_key>(&ModuleMap::change_submod))
      .def("has_submod", &ModuleMap::has_submod)
      .def("default_submod",
           static_cast<default_by_key>(&ModuleMap::default_submod))
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace pluginplay
