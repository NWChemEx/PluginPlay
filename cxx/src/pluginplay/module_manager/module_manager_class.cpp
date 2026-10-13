/*
 * Copyright 2022 NWChemEx-Project
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

#include "detail_/module_manager_pimpl.hpp"
#include "module/detail_/module_pimpl.hpp"
#include <pluginplay/module_manager/module_manager.hpp>

namespace pluginplay {

using module_base_ptr = typename ModuleManager::module_base_ptr;
using runtime_type    = typename ModuleManager::runtime_type;
using runtime_ptr     = typename ModuleManager::runtime_ptr;

ModuleManager::ModuleManager() :
  pimpl_(std::make_unique<detail_::ModuleManagerPIMPL>()) {}
ModuleManager::ModuleManager(runtime_ptr runtime, cache_pointer cache) :
  pimpl_(std::make_unique<detail_::ModuleManagerPIMPL>(runtime, cache)) {}
ModuleManager::ModuleManager(ModuleManager&& rhs) noexcept            = default;
ModuleManager& ModuleManager::operator=(ModuleManager&& rhs) noexcept = default;
ModuleManager::~ModuleManager() noexcept                              = default;

type::size ModuleManager::count(const qualified_key& key) const noexcept {
    return pimpl_->count(key);
}

type::size ModuleManager::size() const noexcept { return pimpl_->size(); }

void ModuleManager::load_plugin(plugin_pointer plugin) {
    pimpl_->load_plugin(std::move(plugin));
}

void ModuleManager::wire_defaults() { pimpl_->wire_defaults(); }

bool ModuleManager::has_plugin(const name_type& name) const noexcept {
    return pimpl_->has_plugin(name);
}

ModuleManager::name_container_type ModuleManager::plugin_names() const {
    return pimpl_->plugin_names();
}

const Plugin& ModuleManager::get_plugin(const name_type& name) const {
    return pimpl_->get_plugin(name);
}

const ModuleMap& ModuleManager::get_modules(const name_type& name) const {
    return const_cast<const detail_::ModuleManagerPIMPL&>(*pimpl_).get_modules(
      name);
}

void ModuleManager::add_module(type::key key, module_base_ptr base) {
    pimpl_->add_module(std::move(key), base);
}

void ModuleManager::erase(const qualified_key& key) { pimpl_->erase(key); }

void ModuleManager::copy_module(const qualified_key& old_key,
                                type::key new_key) {
    pimpl_->copy_module(old_key, std::move(new_key));
}

void ModuleManager::rename_module(const qualified_key& old_key,
                                  type::key new_key) {
    pimpl_->rename_module(old_key, std::move(new_key));
}

void ModuleManager::change_submod(const qualified_key& module_key,
                                  const type::key& callback_key,
                                  const qualified_key& submod_key) {
    pimpl_->at(module_key)->change_submod(callback_key, pimpl_->at(submod_key));
}

const Module& ModuleManager::at(const qualified_key& module_key) const {
    return *pimpl_->at(module_key);
}

void ModuleManager::set_runtime(runtime_ptr runtime) noexcept {
    pimpl_->set_runtime(runtime);
};

ModuleManager::runtime_type& ModuleManager::get_runtime() const noexcept {
    return pimpl_->get_runtime();
};

ModuleManager::key_container_type ModuleManager::keys() const {
    return pimpl_->keys();
}

bool ModuleManager::has_cache() const noexcept {
    // Relies on short-circuiting to not dereference pimpl
    return has_pimpl_() && pimpl_->has_cache();
}

// -----------------------------------------------------------------------------
// -- Private Methods
// -----------------------------------------------------------------------------

bool ModuleManager::has_pimpl_() const noexcept {
    return static_cast<bool>(pimpl_);
}

} // namespace pluginplay
