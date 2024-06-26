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

#include "export_module_manager.hpp"
#include "module/py_module_base.hpp"
#include <parallelzone/parallelzone.hpp>
#include <pluginplay/any/any.hpp>
#include <pluginplay/module_manager/module_manager.hpp>
#include <pluginplay/python/python_wrapper.hpp>
#include <pybind11/stl.h>

namespace pluginplay {

void export_module_manager_class(py_module_reference m) {
    using at_fxn        = Module& (ModuleManager::*)(const type::key&);
    using runtime_type  = typename ModuleManager::runtime_type;
    using cache_type    = typename ModuleManager::cache_type;
    using cache_pointer = typename ModuleManager::cache_pointer;

    using py_obj = pybind11::object;
    using python::PythonWrapper;

    py_class_type<ModuleManager>(m, "ModuleManager")
      .def(pybind11::init<>())
      .def(pybind11::init([](runtime_type rv, cache_pointer cache) {
               auto prv = std::make_shared<runtime_type>(rv);
               return ModuleManager(prv, cache);
           }),
           pybind11::arg("rv"),
           pybind11::arg("cache") = std::make_shared<cache_type>())
      .def("count", &ModuleManager::count)
      .def("size", &ModuleManager::size)
      .def(
        "add_module",
        [](ModuleManager& mm, std::string key,
           std::shared_ptr<PyModuleBase> p) { mm.add_module(key, p); },
        pybind11::keep_alive<1, 3>())
      .def("at", static_cast<at_fxn>(&ModuleManager::at),
           pybind11::return_value_policy::reference_internal)
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
      .def("set_runtime",
           [](ModuleManager& mm, runtime_type rv) {
               mm.set_runtime(std::make_shared<runtime_type>(std::move(rv)));
           })
      .def("get_runtime", &ModuleManager::get_runtime,
           pybind11::return_value_policy::reference_internal)
      .def("keys", &ModuleManager::keys)
      .def("has_cache", &ModuleManager::has_cache)
      .def("__getitem__", [](ModuleManager& self, const type::key& key) {
          return self.at(key);
      });
    m.def("defaulted_mm", []() { return ModuleManager(nullptr); });
}

} // namespace pluginplay
