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

#pragma once
#include <pluginplay/plugin/plugin_class.hpp>

namespace pluginplay {

/** @brief The class Python plugins derive from.
 *
 *  Plugin's constructor is protected, so pybind11 cannot create a Plugin
 *  directly. PyPlugin makes the constructor public. Plugin has no virtual
 *  functions to forward to Python, so PyPlugin adds nothing else.
 */
class PyPlugin : public Plugin {
public:
    PyPlugin() = default;
};

/** @brief Exposes Plugin's protected members to the Python bindings.
 *
 *  Naming a protected member through a class derived from Plugin is allowed,
 *  so the bindings take member pointers through this class.
 */
class PluginPublicist : public Plugin {
public:
    using Plugin::add_module;
    using Plugin::change_submod;
    using Plugin::default_submod_from;
    using Plugin::modules;
    using Plugin::set_name;
    using Plugin::set_preferred_providers;
};

} // namespace pluginplay
