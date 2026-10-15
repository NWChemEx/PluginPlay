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

#pragma once
#include <pluginplay/module_manager/module_manager.hpp>
#include <pluginplay/plugin/plugin_class.hpp>
#include <pluginplay/python/python.hpp>

/** @brief Declares a plugin class.
 *
 *  The resulting class is named @p plugin_name, derives from
 *  pluginplay::Plugin, and has a default constructor. The constructor is
 *  defined with PLUGIN_CTOR.
 *
 *  @param[in] plugin_name The name of the plugin class, *e.g.*, `MyPlugin`.
 */
#define DECLARE_PLUGIN(plugin_name)                 \
    class plugin_name : public pluginplay::Plugin { \
    public:                                         \
        plugin_name();                              \
    }

/** @brief Starts the definition of a plugin's constructor.
 *
 *  The plugin class must derive from pluginplay::Plugin and declare a default
 *  constructor, *e.g.*, by being declared with DECLARE_PLUGIN. The body of the
 *  constructor describes the plugin by calling the protected member functions
 *  of Plugin, *e.g.*, set_name and add_module.
 *
 *  @param[in] plugin_name The name of the plugin class.
 */
#define PLUGIN_CTOR(plugin_name) \
    plugin_name::plugin_name() : pluginplay::Plugin()

/** @brief Creates the Python module for a plugin.
 *
 *  The Python module is named @p module_name and contains the plugin class,
 *  exported under the name @p plugin_name, so Python users can load the plugin
 *  with `mm.load_plugin(module_name.plugin_name())`. The macro is followed by
 *  the body of a function which can export anything else the Python module
 *  needs, *e.g.*, property types:
 *
 *  @code{.cpp}
 *  EXPORT_PLUGIN(my_plugin, MyPlugin, m) {
 *      export_property_types(m);
 *  }
 *  @endcode
 *
 *  Loading a plugin **moves** it into the ModuleManager; so do NOT use the
 *  Python object afterwards. To make the move work, the plugin class also
 *  gets a private `_pp_load_into` method, which ModuleManager.load_plugin
 *  calls; users never call it. The method is compiled into the plugin's
 *  extension library (the one this macro creates), not into PluginPlay's.
 *  This is because pybind11 can only take ownership of an object from code in
 *  the library that created it (the ctor for the plugin will also be in the
 *  same library), when the object is passed as a pointer to its base class
 *  (pybind11 3.1, macOS).
 *
 *  @param[in] module_name The name of the Python module.
 *  @param[in] plugin_name The name of the plugin class.
 *  @param[in] m The name of the pybind11 module variable, for use in the body.
 */
#define EXPORT_PLUGIN(module_name, plugin_name, m)                    \
    void pp_export_##module_name##__(pybind11::module_& m);           \
    PYBIND11_MODULE(module_name, m) {                                 \
        pybind11::module_::import("pluginplay");                      \
        pybind11::class_<plugin_name, pluginplay::Plugin,             \
                         pybind11::smart_holder>(m, #plugin_name)     \
          .def(pybind11::init<>())                                    \
          .def("_pp_load_into", [](std::unique_ptr<plugin_name> self, \
                                   pluginplay::ModuleManager& mm) {   \
              mm.load_plugin(std::move(self));                        \
          });                                                         \
        pp_export_##module_name##__(m);                               \
    }                                                                 \
    void pp_export_##module_name##__(pybind11::module_& m)
