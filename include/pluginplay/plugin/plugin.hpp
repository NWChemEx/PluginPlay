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
#include <pluginplay/module_manager.hpp>
#include <pluginplay/python/python.hpp>

#define DECLARE_PLUGIN(plugin_name) \
    void load_modules(pluginplay::ModuleManager& mm)

#define DEFINE_PLUGIN(plugin_name) \
    inline void load_modules(pluginplay::ModuleManager& mm)

#define EXPORT_PLUGIN(plugin_name, m)                   \
    void pp_export_plugin_name__(pybind11::module_& m); \
    PYBIND11_MODULE(plugin_name, m) {                   \
        m.def("load_modules", &load_modules);           \
        pp_export_plugin_name__(m);                     \
    }                                                   \
    inline void pp_export_plugin_name__(pybind11::module_& m)
