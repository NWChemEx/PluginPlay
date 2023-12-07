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

#include "export_pluginplay.hpp"
#include "py_module_base.hpp"
#include <pluginplay/any/any.hpp>
#include <pluginplay/module_manager.hpp>
#include <pluginplay/python/python_wrapper.hpp>
#include <pybind11/stl.h>

namespace pluginplay {

void export_module_pair(py_module_reference m) {
    py_class_type<std::pair<const std::string, std::shared_ptr<Module>>>(
      m, "ModulePair")
      .def(pybind11::init<>())
      .def_readonly(
        "first", &std::pair<const std::string, std::shared_ptr<Module>>::first)
      .def_readonly(
        "second",
        &std::pair<const std::string, std::shared_ptr<Module>>::second);
}

void export_module_manager(py_module_reference m) {
    using module_base_ptr = ModuleManager::module_base_ptr;
    using at_fxn          = Module& (ModuleManager::*)(const type::key&);
    using add_mod_fxn     = void (ModuleManager::*)(type::key, module_base_ptr);

    using py_obj = pybind11::object;
    using python::PythonWrapper;

    py_class_type<ModuleManager>(m, "ModuleManager")
      .def(pybind11::init<>())
      .def("count", &ModuleManager::count)
      .def("size", &ModuleManager::size)
      .def(
        "add_module",
        [](ModuleManager& mm, std::string key,
           std::shared_ptr<PyModuleBase> p) { mm.add_module(key, p); },
        pybind11::keep_alive<1, 3>())
      .def("at", static_cast<at_fxn>(&ModuleManager::at))
      .def("copy_module", &ModuleManager::copy_module)
      .def("erase", &ModuleManager::erase)
      .def("rename_module", &ModuleManager::rename_module)
      .def("change_input",
           [](ModuleManager& self, std::string mod_key, std::string opt_key,
              py_obj in) {
               self.change_input(mod_key, opt_key,
                                 any::make_any_field<PythonWrapper>(in));
           })
      .def("change_submod", &ModuleManager::change_submod)
      .def("run_as",
           [](py_obj self, py_obj pt, py_obj key, pybind11::args args) {
               return self.attr("at")(key).attr("run_as")(pt, *args);
           })
      .def("keys", &ModuleManager::keys)
      .def("__getitem__", [](ModuleManager& self, const type::key& key) {
          return self.at(key);
      });
    // Python iterator doesn't work, yet. Gives "double free or corruption
    // (out)" error. .def(
    //   "begin",
    //   [](ModuleManager& self) {
    //       return pybind11::make_iterator(self.begin(), self.end());
    //   },
    //   pybind11::keep_alive<0, 1>())
    // .def(
    //   "end",
    //   [](ModuleManager& self) {
    //       return pybind11::make_iterator(self.end(), self.end());
    //   },
    //   pybind11::keep_alive<0, 1>())
    // .def(
    //   "__iter__",
    //   [](ModuleManager& self) {
    //       return pybind11::make_iterator(self.begin(), self.end());
    //   },
    //   pybind11::keep_alive<0, 1>());
    //.def("set_runtime", &ModuleManager::set_runtime)
    //.def("get_runtime", &ModuleManager::get_runtime)
    m.def("defaulted_mm", []() { return ModuleManager(nullptr); });
}

} // namespace pluginplay
