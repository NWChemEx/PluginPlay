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
#include "../module_manager/module_map_view.hpp"
#include <pluginplay/plugin/plugin_class.hpp>

namespace pluginplay {

/** @brief A read-only view of a Plugin, for the Python bindings.
 *
 *  ModuleManager::get_plugin returns a `const Plugin&`. Python has no const,
 *  and the Plugin bindings include the setters a plugin written in Python
 *  uses in its constructor, so a loaded plugin returned to Python as a
 *  Plugin could be changed. ModuleManager.get_plugin returns a PluginView
 *  instead, which only exposes Plugin's read-only members.
 *
 *  The view does not own the plugin; the bindings keep the plugin's owner
 *  alive for as long as the view exists.
 */
class PluginView {
public:
    /// Type used for plugin names
    using name_type = Plugin::name_type;

    /// Type of a list of plugin names
    using provider_list_type = Plugin::provider_list_type;

    /// Type of a list of recorded default submodules
    using default_list_type = Plugin::default_list_type;

    /// Creates a view of @p plugin. No throw guarantee.
    explicit PluginView(const Plugin& plugin) noexcept : m_plugin_(&plugin) {}

    /// Forwards to Plugin::name
    const name_type& name() const noexcept { return m_plugin_->name(); }

    /// Forwards to Plugin::preferred_providers
    const provider_list_type& preferred_providers() const noexcept {
        return m_plugin_->preferred_providers();
    }

    /// Forwards to Plugin::default_submods_from
    const default_list_type& default_submods_from(
      const name_type& provider) const noexcept {
        return m_plugin_->default_submods_from(provider);
    }

    /// A read-only view of the plugin's modules
    ModuleMapView get_modules() const noexcept {
        return ModuleMapView(m_plugin_->get_modules());
    }

    /// True if the viewed plugins are equal
    bool operator==(const PluginView& rhs) const {
        return *m_plugin_ == *rhs.m_plugin_;
    }

    /// Negates operator==
    bool operator!=(const PluginView& rhs) const { return !(*this == rhs); }

private:
    /// The plugin *this views
    const Plugin* m_plugin_;
};

} // namespace pluginplay
