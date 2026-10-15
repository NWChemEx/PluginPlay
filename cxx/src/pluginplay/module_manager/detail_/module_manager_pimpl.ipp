/*
 * Copyright 2024 NWChemEx-Project
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

// This file meant only for inclusion in module_manager_pimpl.hpp

#pragma once

namespace pluginplay::detail_ {

inline type::size ModuleManagerPIMPL::count(
  const qualified_key& key) const noexcept {
    if(key.is_qualified()) {
        const auto* map = find_map_(key.plugin());
        return map ? map->count(key.key()) : 0;
    }
    auto rv = m_modules.count(key.key());
    for(const auto& plugin : m_plugins)
        rv += plugin->m_modules_.count(key.key());
    return rv;
}

inline type::size ModuleManagerPIMPL::size() const noexcept {
    auto rv = m_modules.size();
    for(const auto& plugin : m_plugins) rv += plugin->m_modules_.size();
    return rv;
}

inline void ModuleManagerPIMPL::add_module(type::key key,
                                           module_base_ptr base) {
    if(m_modules.count(key)) throw std::invalid_argument("Key is in use");
    if(!base)
        throw std::invalid_argument("Module implementation for key: '" + key +
                                    "' is null");

    module_base_ptr stored = base;
    if(base->is_python()) {
        // This is a hacky patch to allow multiple python modules to be
        // added while avoiding the type_index collisions.
        // TODO: remove when a more elegant solution is determined
        m_py_bases[key] = base;
    } else {
        std::type_index type(base->type());
        if(!m_bases.count(type)) m_bases[type] = base;
        stored = m_bases[type];
    }

    Module mod(std::make_unique<ModulePIMPL>(stored));
    attach_(key, mod);
    m_modules.add_module(std::move(key), std::move(mod));
}

inline void ModuleManagerPIMPL::erase(const qualified_key& key) {
    if(!count(key)) return;
    get_modules(resolve_(key)).erase(key.key());
}

inline void ModuleManagerPIMPL::copy_module(const qualified_key& old_key,
                                            type::key new_key) {
    m_modules.add_module(std::move(new_key), at(old_key)->unlocked_copy());
}

inline void ModuleManagerPIMPL::rename_module(const qualified_key& old_key,
                                              type::key new_key) {
    // Pin down which module is renamed before the copy can make old_key
    // ambiguous
    const qualified_key resolved(resolve_(old_key), old_key.key());
    copy_module(resolved, std::move(new_key));
    erase(resolved);
}

inline ModuleManagerPIMPL::shared_module ModuleManagerPIMPL::at(
  const qualified_key& key) const {
    return get_modules(resolve_(key)).at(key.key());
}

inline void ModuleManagerPIMPL::load_plugin(plugin_pointer plugin) {
    if(!plugin) throw std::invalid_argument("Can not load a null plugin");

    const auto& name = plugin->name();
    if(name.empty())
        throw std::invalid_argument(
          "Plugins must have a name. Call set_name in the plugin's "
          "constructor.");
    if(has_plugin(name))
        throw std::invalid_argument("A plugin named '" + name +
                                    "' is already loaded");

    // Attach the plugin's modules, then every module reachable from them
    // through submodules. A plugin may set a submodule and then erase that
    // module's key; the module is still used, so it needs the runtime and
    // caches too.
    std::set<const Module*> visited;
    std::vector<Module*> to_visit;
    for(auto& [key, mod] : plugin->m_modules_) {
        attach_(qualified_key(name, key).to_string(), *mod);
        visited.insert(mod.get());
        to_visit.push_back(mod.get());
    }
    while(!to_visit.empty()) {
        auto* mod = to_visit.back();
        to_visit.pop_back();
        for(auto& [callback, request] : mod->m_pimpl_->submods()) {
            if(!request.has_module()) continue;
            auto& submod = request.value();
            if(!visited.insert(&submod).second) continue;

            // The module is no longer in the plugin's map, so its cache is
            // named after the key it was last stored under
            const auto& last_key =
              submod.has_name() ? submod.get_name() : callback;
            attach_(qualified_key(name, last_key).to_string(), submod);
            to_visit.push_back(&submod);
        }
    }

    m_plugins.push_back(std::move(plugin));
}

inline void ModuleManagerPIMPL::wire_defaults() {
    for(const auto& plugin : m_plugins) {
        auto& modules = plugin->m_modules_;
        for(const auto& provider_name : plugin->preferred_providers()) {
            const auto* provider = find_plugin_(provider_name);
            if(provider == nullptr) continue;

            const auto& provided = provider->m_modules_;
            for(const auto& record :
                plugin->default_submods_from(provider_name)) {
                // The user may have erased the module after loading
                if(!modules.count(record.module_key)) continue;

                if(!provided.count(record.provider_key)) {
                    if(m_runtime_)
                        m_runtime_->logger().warn(
                          "Plugin '" + plugin->name() + "' asked for module '" +
                          record.provider_key + "' from plugin '" +
                          provider_name + "', which does not have it.");
                    continue;
                }

                modules.default_submod(record.module_key, record.callback_key,
                                       provided.at(record.provider_key));
            }
        }
    }
}

inline bool ModuleManagerPIMPL::has_plugin(
  const name_type& name) const noexcept {
    return find_plugin_(name) != nullptr;
}

inline ModuleManagerPIMPL::name_container_type
ModuleManagerPIMPL::plugin_names() const {
    name_container_type rv{""};
    rv.reserve(m_plugins.size() + 1);
    for(const auto& plugin : m_plugins) rv.push_back(plugin->name());
    return rv;
}

inline const Plugin& ModuleManagerPIMPL::get_plugin(
  const name_type& name) const {
    const auto* plugin = find_plugin_(name);
    if(plugin == nullptr)
        throw std::out_of_range("ModuleManager has no plugin named: '" + name +
                                "'");
    return *plugin;
}

inline ModuleManagerPIMPL::module_map& ModuleManagerPIMPL::get_modules(
  const name_type& name) {
    const auto& const_this = const_cast<const ModuleManagerPIMPL&>(*this);
    return const_cast<module_map&>(const_this.get_modules(name));
}

inline const ModuleManagerPIMPL::module_map& ModuleManagerPIMPL::get_modules(
  const name_type& name) const {
    const auto* map = find_map_(name);
    if(map == nullptr)
        throw std::out_of_range("ModuleManager has no plugin named: '" + name +
                                "'");
    return *map;
}

inline bool ModuleManagerPIMPL::operator==(
  const ModuleManagerPIMPL& rhs) const {
    // Try to get out early
    if(m_bases.size() != rhs.m_bases.size()) return false;
    if(m_modules.size() != rhs.m_modules.size()) return false;
    if(m_plugins.size() != rhs.m_plugins.size()) return false;

    // TODO: Remove with the rest of the python hack
    if(m_py_bases.size() != rhs.m_py_bases.size()) return false;
    for(const auto& [k, v] : rhs.m_py_bases) {
        if(!m_py_bases.count(k)) return false;
        if(*m_py_bases.at(k) != *v) return false;
    }

    // Skip checking the values b/c implementations are compared by type
    for(const auto& [k, v] : rhs.m_bases) {
        if(!m_bases.count(k)) return false;
    }

    // Need to check the values b/c user may have switched options
    if(m_modules != rhs.m_modules) return false;

    // Load order does not matter, so plugins are matched by name
    for(const auto& plugin : rhs.m_plugins) {
        const auto* mine = find_plugin_(plugin->name());
        if(mine == nullptr || *mine != *plugin) return false;
    }

    return true;
}

inline ModuleManagerPIMPL::key_container_type ModuleManagerPIMPL::keys() const {
    key_container_type rv;
    rv.reserve(size());
    for(const auto& name : plugin_names())
        for(const auto& key : get_modules(name).keys())
            rv.emplace_back(name, key);
    return rv;
}

// -----------------------------------------------------------------------------
// -- Private Methods
// -----------------------------------------------------------------------------

inline Plugin* ModuleManagerPIMPL::find_plugin_(
  const name_type& name) const noexcept {
    using utilities::strings::case_insensitive_equal;
    for(const auto& plugin : m_plugins)
        if(case_insensitive_equal(plugin->name(), name)) return plugin.get();
    return nullptr;
}

inline const ModuleManagerPIMPL::module_map* ModuleManagerPIMPL::find_map_(
  const name_type& name) const noexcept {
    if(name.empty()) return &m_modules;
    const auto* plugin = find_plugin_(name);
    return plugin ? &plugin->m_modules_ : nullptr;
}

inline ModuleManagerPIMPL::name_type ModuleManagerPIMPL::resolve_(
  const qualified_key& key) const {
    const std::string no_module =
      "ModuleManager has no module with key: '" + key.to_string() + "'";

    if(key.is_qualified()) {
        const auto* map = find_map_(key.plugin());
        if(map == nullptr)
            throw std::out_of_range("ModuleManager has no plugin named: '" +
                                    key.plugin() + "'");
        if(!map->count(key.key())) throw std::out_of_range(no_module);
        return key.plugin();
    }

    name_container_type holders;
    for(const auto& name : plugin_names())
        if(find_map_(name)->count(key.key())) holders.push_back(name);

    if(holders.empty()) throw std::out_of_range(no_module);
    if(holders.size() > 1) {
        std::string msg = "Module key '" + key.key() +
                          "' is ambiguous. It is used by the plugins:";
        for(const auto& name : holders)
            msg += name.empty() ? " '' (the ModuleManager's own modules)" :
                                  " '" + name + "'";
        msg += ". Pass the plugin name too, e.g., {\"" + holders.back() +
               "\", \"" + key.key() + "\"}.";
        throw std::invalid_argument(msg);
    }
    return holders.front();
}

inline void ModuleManagerPIMPL::attach_(const type::key& cache_key,
                                        Module& mod) const {
    mod.m_pimpl_->attach(m_runtime_, m_pcaches, cache_key);
}

} // namespace pluginplay::detail_
