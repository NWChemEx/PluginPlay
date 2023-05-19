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
#include "detail_/module_pimpl.hpp"
#include "pluginplay/module_manager.hpp"

namespace pluginplay {

using module_map      = typename ModuleManager::module_map;
using module_base_ptr = typename ModuleManager::module_base_ptr;
using cache_type      = typename detail_::ModulePIMPL::cache_type;
using runtime_type    = typename ModuleManager::runtime_type;
using runtime_ptr     = typename ModuleManager::runtime_ptr;
template<typename T>
using CIM = utilities::CaseInsensitiveMap<T>;

ModuleManager::ModuleManager() :
  pimpl_(std::make_unique<detail_::ModuleManagerPIMPL>()) {}
ModuleManager::ModuleManager(runtime_ptr runtime) :
  pimpl_(std::make_unique<detail_::ModuleManagerPIMPL>(runtime)) {}
ModuleManager::ModuleManager(ModuleManager&& rhs) noexcept = default;
ModuleManager& ModuleManager::operator=(ModuleManager&& rhs) noexcept = default;
ModuleManager::~ModuleManager() noexcept                              = default;

type::size ModuleManager::count(type::key key) const noexcept {
    return pimpl_->count(key);
}

void ModuleManager::add_module(type::key key, module_base_ptr base) {
    pimpl_->add_module(std::move(key), base);
}

void ModuleManager::erase(const type::key& key) { pimpl_->erase(key); }

void ModuleManager::copy_module(const type::key& old_key, type::key new_key) {
    pimpl_->copy_module(old_key, std::move(new_key));
}

void ModuleManager::change_submod(const type::key& module_key,
                                  const type::key& callback_key,
                                  const type::key& submod_key) {
    pimpl_->at(module_key)->change_submod(callback_key, pimpl_->at(submod_key));
}

const Module& ModuleManager::at(const type::key& module_key) const {
    return *pimpl_->at(module_key);
}

void ModuleManager::set_default_(const std::type_info& type,
                                 type::input_map inps, type::key key) {
    pimpl_->set_default(type, std::move(inps), std::move(key));
}

module_map::iterator ModuleManager::begin() noexcept { return pimpl_->begin(); }

module_map::iterator ModuleManager::end() noexcept { return pimpl_->end(); }

type::size ModuleManager::size() const noexcept { return pimpl_->size(); }

void ModuleManager::set_runtime(runtime_ptr runtime) noexcept {
    pimpl_->set_runtime(runtime);
};

ModuleManager::runtime_type& ModuleManager::get_runtime() const noexcept {
    return pimpl_->get_runtime();
};

} // namespace pluginplay
