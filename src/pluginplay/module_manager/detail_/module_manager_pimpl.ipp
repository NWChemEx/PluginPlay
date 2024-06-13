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

inline void ModuleManagerPIMPL::set_default(const std::type_info& type,
                                            type::input_map inputs,
                                            type::key key) {
    if(!count(key)) m_modules.at(key); // Throws a consistent error
    m_defaults[std::type_index(type)] = key;
    m_inputs[std::type_index(type)]   = std::move(inputs);
}

inline void ModuleManagerPIMPL::add_module(type::key key,
                                           module_base_ptr base) {
    assert_unique_key_(key);
    auto uuid = utility::generate_uuid();
    base->set_runtime(m_runtime_);
    base->set_uuid(uuid);

    cache::ModuleManagerCache::module_cache_pointer module_cache;
    if(m_pcaches) {
        auto internal_cache = m_pcaches->get_or_make_user_cache(uuid);
        base->set_cache(internal_cache);
        module_cache = m_pcaches->get_or_make_module_cache(key);
    }

    std::unique_ptr<ModulePIMPL> pimpl;
    if(base->is_python()) {
        // This is a hacky patch to allow multiple python modules to be
        // added while avoiding the type_index collisions.
        // TODO: remove when a more elegant solution is determined
        m_py_bases[key] = base;
        pimpl = std::make_unique<ModulePIMPL>(m_py_bases[key], module_cache);
    } else {
        std::type_index type(base->type());
        if(!m_bases.count(type)) m_bases[type] = base;
        pimpl = std::make_unique<ModulePIMPL>(m_bases[type], module_cache);
    }
    auto ptr = std::make_shared<Module>(std::move(pimpl));
    ptr->set_name(key);
    m_modules.emplace(std::move(key), ptr);
}

inline void ModuleManagerPIMPL::copy_module(const type::key& old_key,
                                            type::key new_key) {
    assert_unique_key_(new_key);
    Module mod = m_modules.at(old_key)->unlocked_copy();
    auto ptr   = std::make_shared<Module>(std::move(mod));
    ptr->set_name(new_key);
    m_modules.emplace(std::move(new_key), ptr);
}

inline ModuleManagerPIMPL::shared_module ModuleManagerPIMPL::at(
  const type::key& key) {
    if(!count(key)) {
        const std::string msg =
          "ModuleManager has no module with key: '" + key + "'";
        throw std::out_of_range(msg);
    }
    auto mod = m_modules.at(key);
    // Loop over submodules filling them in from the defaults
    for(auto& [k, v] : mod->submods()) {
        const auto& type = v.type();
        // Only change non-ready submodules
        if(!v.ready() && m_defaults.count(type)) {
            // Recursive to make sure that that module gets filled in
            auto default_mod = at(m_defaults.at(type));
            // Only change if the module is also ready
            if(default_mod->ready(m_inputs.at(type)))
                mod->change_submod(k, default_mod);
        }
    }
    return mod;
}

inline bool ModuleManagerPIMPL::operator==(
  const ModuleManagerPIMPL& rhs) const {
    // Try to get out early
    if(m_bases.size() != rhs.m_bases.size()) return false;
    if(m_modules.size() != rhs.m_modules.size()) return false;
    if(m_defaults.size() != rhs.m_defaults.size()) return false;

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
    for(const auto& [k, v] : rhs.m_modules) {
        if(!m_modules.count(k)) return false;
        if(*m_modules.at(k) != *v) return false;
    }

    // Easy since not pointers
    if(m_defaults != rhs.m_defaults) return false;

    return true;
}

inline ModuleManager::key_container_type ModuleManagerPIMPL::keys() const {
    ModuleManager::key_container_type keys;
    keys.reserve(m_modules.size());
    for(const auto& [k, v] : m_modules) keys.push_back(k);
    return keys;
}

} // namespace pluginplay::detail_