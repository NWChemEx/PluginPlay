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
#include "export_plugin.hpp"
#include "plugin_view.hpp"
#include "py_plugin.hpp"

namespace pluginplay {

void export_plugin_class(py_module_reference m) {
    using name_type          = Plugin::name_type;
    using key_type           = Plugin::key_type;
    using module_base_ptr    = Plugin::module_base_ptr;
    using provider_list_type = Plugin::provider_list_type;
    using default_submod     = Plugin::DefaultSubmod;

    // Member pointers to the protected members, resolved through the
    // publicist. The overloaded members need their overload picked.
    using add_module_fxn = void (Plugin::*)(key_type, module_base_ptr);
    using providers_fxn  = void (Plugin::*)(provider_list_type);
    using modules_fxn    = ModuleMap& (Plugin::*)();
    auto add_module = static_cast<add_module_fxn>(&PluginPublicist::add_module);
    auto set_providers =
      static_cast<providers_fxn>(&PluginPublicist::set_preferred_providers);
    auto modules = static_cast<modules_fxn>(&PluginPublicist::modules);

    py_class_type<PluginView>(m, "PluginView")
      .def("name", &PluginView::name)
      .def("preferred_providers", &PluginView::preferred_providers)
      .def("default_submods_from", &PluginView::default_submods_from)
      .def("get_modules", &PluginView::get_modules, py::keep_alive<0, 1>())
      .def(py::self == py::self)
      .def(py::self != py::self);

    auto py_plugin =
      py_class_type<Plugin, PyPlugin, py::smart_holder>(m, "Plugin");

    py_class_type<default_submod>(py_plugin, "DefaultSubmod")
      .def_readonly("module_key", &default_submod::module_key)
      .def_readonly("callback_key", &default_submod::callback_key)
      .def_readonly("provider_key", &default_submod::provider_key)
      .def(py::self == py::self)
      .def(py::self != py::self);

    py_plugin.def(py::init_alias<>())
      .def("name", &Plugin::name)
      .def("preferred_providers", &Plugin::preferred_providers)
      .def("default_submods_from", &Plugin::default_submods_from)
      .def(
        "get_modules",
        [](const Plugin& self) { return ModuleMapView(self.get_modules()); },
        py::keep_alive<0, 1>())
      .def("set_name", &PluginPublicist::set_name)
      .def(
        "add_module",
        [add_module](Plugin& self, key_type key,
                     std::shared_ptr<PyModuleBase> p) {
            (self.*add_module)(std::move(key), p);
        },
        py::keep_alive<1, 3>())
      .def("change_submod", &PluginPublicist::change_submod)
      .def("set_preferred_providers",
           [set_providers](Plugin& self, py::args names) {
               provider_list_type providers;
               for(const auto& name : names)
                   providers.push_back(name.cast<name_type>());
               (self.*set_providers)(std::move(providers));
           })
      .def("default_submod_from", &PluginPublicist::default_submod_from)
      .def(
        "modules",
        [modules](Plugin& self) -> ModuleMap& { return (self.*modules)(); },
        py::return_value_policy::reference_internal)
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace pluginplay
