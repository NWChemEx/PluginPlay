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

#include "../module/py_module_base.hpp"
#include "export_module_manager.hpp"
#include <parallelzone/parallelzone.hpp>
#include <pluginplay/any/any.hpp>
#include <pluginplay/module_manager/module_manager.hpp>
#include <pluginplay/python/python_wrapper.hpp>

namespace pluginplay {

void export_module_manager_class(py_module_reference m) {
    using qualified_key = typename ModuleManager::qualified_key;
    using at_fxn        = Module& (ModuleManager::*)(const qualified_key&);
    using plugin_ptr    = typename ModuleManager::plugin_pointer;
    using load_fxn      = void (ModuleManager::*)(plugin_ptr);
    using runtime_type  = typename ModuleManager::runtime_type;
    using cache_type    = typename ModuleManager::cache_type;
    using cache_pointer = typename ModuleManager::cache_pointer;

    using py_obj = py::object;
    using python::PythonWrapper;

    py_module_type::import("parallelzone"); // For runtime interface

    py_class_type<ModuleManager>(m, "ModuleManager")
      .def(py::init<>())
      .def(py::init([](runtime_type rv, cache_pointer cache) {
               auto prv = std::make_shared<runtime_type>(rv);
               return ModuleManager(prv, cache);
           }),
           py::arg("rv"), py::arg("cache") = std::make_shared<cache_type>())
      .def("count", &ModuleManager::count)
      .def("size", &ModuleManager::size)
      .def("load_plugin",
           [](py_obj self, py_obj plugin) {
               // C++ plugins from other extension modules release ownership
               // in their own module (see EXPORT_PLUGIN)
               if(py::hasattr(plugin, "_pp_load_into")) {
                   plugin.attr("_pp_load_into")(self);
                   return;
               }
               auto& mm = self.cast<ModuleManager&>();
               mm.load_plugin(plugin.cast<plugin_ptr>());
           })
      .def("wire_defaults", &ModuleManager::wire_defaults)
      .def("has_plugin", &ModuleManager::has_plugin)
      .def("plugin_names", &ModuleManager::plugin_names)
      .def("get_plugin", &ModuleManager::get_plugin,
           py::return_value_policy::reference_internal)
      .def("get_modules", &ModuleManager::get_modules,
           py::return_value_policy::reference_internal)
      .def(
        "add_module",
        [](ModuleManager& mm, std::string key,
           std::shared_ptr<PyModuleBase> p) { mm.add_module(key, p); },
        py::keep_alive<1, 3>())
      .def("at", static_cast<at_fxn>(&ModuleManager::at),
           py::return_value_policy::reference_internal)
      .def("copy_module", &ModuleManager::copy_module)
      .def("erase", &ModuleManager::erase)
      .def("rename_module", &ModuleManager::rename_module)
      .def("change_input",
           [](ModuleManager& self, const qualified_key& mod_key,
              std::string opt_key, py_obj in) {
               self.change_input(mod_key, opt_key,
                                 any::make_any_field<PythonWrapper>(in));
           })
      .def("change_submod", &ModuleManager::change_submod)
      .def("run_as",
           [](py_obj self, py_obj pt, py_obj key, py::args args) {
               return self.attr("at")(key).attr("run_as")(pt, *args);
           })
      .def("set_runtime",
           [](ModuleManager& mm, runtime_type rv) {
               mm.set_runtime(std::make_shared<runtime_type>(std::move(rv)));
           })
      .def("get_runtime", &ModuleManager::get_runtime,
           py::return_value_policy::reference_internal)
      .def("keys", &ModuleManager::keys)
      .def("has_cache", &ModuleManager::has_cache)
      .def("__getitem__", [](ModuleManager& self, const qualified_key& key) {
          return self.at(key);
      });
    m.def("defaulted_mm", []() { return ModuleManager(nullptr); });
}

} // namespace pluginplay
