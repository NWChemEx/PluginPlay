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

#include "../module/detail_/module_pimpl.hpp"
#include <pluginplay/module_manager/module_map.hpp>
#include <stdexcept>

namespace pluginplay {

void ModuleMap::add_module(key_type key, module_base_ptr base) {
    if(count(key))
        throw std::invalid_argument(
          "ModuleMap already has a module with key: '" + key + "'");
    if(!base)
        throw std::invalid_argument("Module implementation for key: '" + key +
                                    "' is null");

    auto pimpl = std::make_unique<detail_::ModulePIMPL>(base);
    add_module(std::move(key), Module(std::move(pimpl)));
}

void ModuleMap::add_module(key_type key, Module module) {
    if(count(key))
        throw std::invalid_argument(
          "ModuleMap already has a module with key: '" + key + "'");

    auto ptr = std::make_shared<Module>(std::move(module));
    ptr->set_name(key);
    m_modules_.emplace(std::move(key), std::move(ptr));
}

void ModuleMap::copy_module(const key_type& old_key, key_type new_key) {
    add_module(std::move(new_key), at(old_key)->unlocked_copy());
}

ModuleMap::module_pointer ModuleMap::at(const key_type& key) const {
    if(!count(key))
        throw std::out_of_range("ModuleMap has no module with key: '" + key +
                                "'");
    return m_modules_.at(key);
}

ModuleMap::key_container_type ModuleMap::keys() const {
    key_container_type rv;
    rv.reserve(size());
    for(const auto& [k, v] : m_modules_) rv.push_back(k);
    return rv;
}

void ModuleMap::change_submod(const key_type& module_key,
                              const key_type& callback_key,
                              const key_type& submod_key) {
    change_submod(module_key, callback_key, at(submod_key));
}

void ModuleMap::change_submod(const key_type& module_key,
                              const key_type& callback_key,
                              module_pointer submod) {
    at(module_key)->change_submod(callback_key, std::move(submod));
}

bool ModuleMap::has_submod(const key_type& module_key,
                           const key_type& callback_key) const {
    return at(module_key)->submods().at(callback_key).has_module();
}

bool ModuleMap::default_submod(const key_type& module_key,
                               const key_type& callback_key,
                               const key_type& submod_key) {
    if(has_submod(module_key, callback_key)) return false;
    change_submod(module_key, callback_key, submod_key);
    return true;
}

bool ModuleMap::default_submod(const key_type& module_key,
                               const key_type& callback_key,
                               module_pointer submod) {
    if(has_submod(module_key, callback_key)) return false;
    change_submod(module_key, callback_key, std::move(submod));
    return true;
}

bool ModuleMap::operator==(const ModuleMap& rhs) const {
    if(size() != rhs.size()) return false;
    for(const auto& [k, v] : rhs.m_modules_) {
        if(!count(k)) return false;
        if(*m_modules_.at(k) != *v) return false;
    }
    return true;
}

} // namespace pluginplay
