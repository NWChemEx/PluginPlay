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

#pragma once
#include "../../module/detail_/module_pimpl.hpp"
#include <memory>
#include <parallelzone/runtime/runtime_view.hpp>
#include <pluginplay/cache/module_manager_cache.hpp>
#include <pluginplay/module/module_base.hpp>
#include <pluginplay/module_manager/module_manager.hpp>
#include <typeindex>

namespace pluginplay::detail_ {

/** @brief The class that implements the ModuleManager.
 *
 *  Users are expected to go through the API provided by the ModuleManager
 *  class. Inside the ModuleManager class calls are redirected to this class.
 *  When using this class calls should go through the member functions as much
 *  as possible so that error checking occurs. That said members are public so
 *  that it is easier to test the implementation.
 */
struct ModuleManagerPIMPL {
    /// Type *this implements
    using module_manager_type = ModuleManager;

    ///@{
    /// Type of a pointer to a module's implemenation
    using module_base_ptr = module_manager_type::module_base_ptr;

    /// Type of a pointer to a read-only module implementation
    using const_module_base_ptr = module_manager_type::const_module_base_ptr;

    /// Type of a map from the module implementation's type to the
    /// implementation
    using base_map = std::map<std::type_index, const_module_base_ptr>;

    /// Type of a usable module
    using shared_module = std::shared_ptr<Module>;

    /// Type of a map holding usable modules
    using module_map = module_manager_type::module_map;

    /// Type of a map holding the default module key for a given property type
    using default_map = std::map<std::type_index, type::key>;

    /// The type of the runtime
    using runtime_type = module_manager_type::runtime_type;

    /// A pointer to a runtime
    using runtime_ptr = module_manager_type::runtime_ptr;

    /// Type of the cache
    using cache_type = module_manager_type::cache_type;

    /// Type of a pointer to the cache
    using cache_pointer = module_manager_type::cache_pointer;

    /// Type of a map from key to Python implementation
    // TODO: remove when a more elegant solution is determined
    using py_base_map = std::map<type::key, const_module_base_ptr>;

    ///@}

    ModuleManagerPIMPL() :
      ModuleManagerPIMPL(std::make_shared<runtime_type>(),
                         std::make_shared<cache_type>()) {}

    ModuleManagerPIMPL(runtime_ptr runtime, cache_pointer cache) :
      m_pcaches(cache), m_runtime_(runtime) {}

    /// Makes a deep copy of this instance on the heap
    // auto clone() { return std::make_unique<ModuleManagerPIMPL>(*this); }

    /// Ensures we determine if we have a module consistently
    type::size count(const type::key& key) const noexcept {
        return m_modules.count(key);
    }

    /// Ensures we count the number of modules consistently
    type::size size() const noexcept { return m_modules.size(); }

    /** @brief Returns an iterator to the first element of the module map
     *
     * @return Iterator to the first element of the map
     */
    module_map::iterator begin() noexcept { return m_modules.begin(); };

    /** @brief Returns an iterator to the past-the-end element of the module map
     *
     * @return Iterator to the past-the-end element of the map
     */
    module_map::iterator end() noexcept { return m_modules.end(); };

    /** @brief Sets the default module to use for a given property type
     *
     * When a user requests a module that module initially has no submodules
     * set unless the user has bound some to that module. Instead, the
     * ModuleManager is now responsible to provide the module with submodules
     * using the defaults. This function allows one to specify what the
     * defaults are.
     *
     * @param type The type of the property type this default is for
     * @param key The module key for the module to use as the default
     */
    void set_default(const std::type_info& type, type::input_map inputs,
                     type::key key);

    /** @brief This function actually adds a module to the list of available
     *         modules.
     *
     * @param key The key under which the module will be registered.
     * @param base The instance containing the algorithm
     */
    void add_module(type::key key, module_base_ptr base);

    /** @brief Unloads the specified module.
     *
     *  This function unloads the module with the specified key. After this
     *  operation the key is free to be used again. Calling this function does
     *  NOT clean any data out of the cache. This function is a no-op if @p key
     *  does not exist.
     *
     *  @param[in] key The key for the module which should be erased.
     *
     *  @throw None No throw guarantee.
     */
    void erase(const type::key& key) { m_modules.erase(key); }

    /** @brief Makes a deep copy of a module
     *
     * This function makes a deep copy of a module. The new module is unlocked
     * regardless of whether the old module was locked or not. The user can call
     * lock on the resulting module to make an exact copy
     * @param old_key The key for the module to copy
     * @param new_key The key under which the new module will live
     */
    void copy_module(const type::key& old_key, type::key new_key);

    /** @brief Returns a module, filling in all non-set submodules with defaults
     *         if a ready default exists.
     *
     * @param key The module you want
     * @return A shared_ptr to the requested module
     */
    shared_module at(const type::key& key);

    ///@{
    /** @name Comparison operators
     *
     * @param rhs
     * @return
     */
    bool operator==(const ModuleManagerPIMPL& rhs) const;

    bool operator!=(const ModuleManagerPIMPL& rhs) const {
        return !((*this) == rhs);
    }

    void set_runtime(runtime_ptr runtime) noexcept { m_runtime_ = runtime; }

    runtime_type& get_runtime() const { return *m_runtime_; }

    ModuleManager::key_container_type keys() const;

    bool has_cache() const noexcept { return static_cast<bool>(m_pcaches); }
    ///@}

    ///@{
    /** @name ModuleManager state
     *
     *  The members in this section are the state of the ModuleManager class.
     */
    // These are the Modules in the states provided to us by the developer
    base_map m_bases;

    // These are the Modules in the state set by the user
    module_map m_modules;

    // Part of the hacky patch to make multiple python modules work
    // TODO: remove when a more elegant solution is determined
    // These are the Python Modules in their developer state
    py_base_map m_py_bases;

    // These are the results of the modules running in the user's states
    cache_pointer m_pcaches;

    // A map of property types
    default_map m_defaults;

    // A map of inputs for property types
    std::map<std::type_index, type::input_map> m_inputs;

    // Pointer to this modules current runtime
    runtime_ptr m_runtime_;
    ///@}
private:
    /// Wraps the check for making sure @p key is not in use.
    void assert_unique_key_(const type::key& key) const {
        if(count(key)) throw std::invalid_argument("Key is in use");
    }
};

} // namespace pluginplay::detail_

#include "module_manager_pimpl.ipp"
