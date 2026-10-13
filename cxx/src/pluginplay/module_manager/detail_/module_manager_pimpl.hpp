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
#include <pluginplay/plugin/plugin_class.hpp>
#include <typeindex>
#include <utilities/strings/string_tools.hpp>
#include <utility>
#include <vector>

namespace pluginplay::detail_ {

/** @brief The class that implements the ModuleManager.
 *
 *  Users are expected to go through the API provided by the ModuleManager
 *  class. Inside the ModuleManager class calls are redirected to this class.
 *  When using this class calls should go through the member functions as much
 *  as possible so that error checking occurs. That said members are public so
 *  that it is easier to test the implementation.
 *
 *  The modules are stored in one ModuleMap per loaded plugin, plus a ModuleMap
 *  for the modules that do not come from a plugin (the "own" map). The own map
 *  is identified by the empty plugin name.
 */
struct ModuleManagerPIMPL {
    /// Type *this implements
    using module_manager_type = ModuleManager;

    ///@{
    /// Type of a pointer to a module's implemenation
    using module_base_ptr = module_manager_type::module_base_ptr;

    /// Type of a map from the module implementation's type to the
    /// implementation
    using base_map = std::map<std::type_index, module_base_ptr>;

    /// Type of a usable module
    using shared_module = std::shared_ptr<Module>;

    /// Type of a map holding usable modules
    using module_map = ModuleMap;

    /// Type used to identify modules
    using qualified_key = QualifiedKey;

    /// Type used for the names of plugins
    using name_type = typename module_manager_type::name_type;

    /// Type of a container of plugin names
    using name_container_type =
      typename module_manager_type::name_container_type;

    /// Type of a container of qualified keys
    using key_container_type = typename module_manager_type::key_container_type;

    /// Type of a pointer to a loaded plugin
    using plugin_pointer = typename module_manager_type::plugin_pointer;

    /// Type of the container holding the loaded plugins
    using plugin_container_type = std::vector<plugin_pointer>;

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
    using py_base_map = std::map<type::key, module_base_ptr>;

    ///@}

    ModuleManagerPIMPL() :
      ModuleManagerPIMPL(std::make_shared<runtime_type>(),
                         std::make_shared<cache_type>()) {}

    ModuleManagerPIMPL(runtime_ptr runtime, cache_pointer cache) :
      m_pcaches(cache), m_runtime_(runtime) {}

    /** @brief The number of modules @p key refers to.
     *
     *  For a key-only @p key this is the number of maps that contain the key.
     *  For a qualified @p key this is 1 if the plugin is loaded and has the
     *  module and 0 otherwise.
     */
    type::size count(const qualified_key& key) const noexcept;

    /// The total number of modules in all maps
    type::size size() const noexcept;

    /** @brief This function actually adds a module to the list of available
     *         modules.
     *
     *  The module is added to the own map.
     *
     * @param key The key under which the module will be registered.
     * @param base The instance containing the algorithm
     */
    void add_module(type::key key, module_base_ptr base);

    /** @brief Unloads the specified module.
     *
     *  This function unloads the module @p key refers to. After this operation
     *  the key is free to be used again in that map. Calling this function
     *  does NOT clean any data out of the cache. This function is a no-op if
     *  @p key does not refer to a module.
     *
     *  @param[in] key The module which should be erased.
     *
     *  @throw std::invalid_argument if @p key is key-only and more than one
     *                               map has the key. Strong throw guarantee.
     */
    void erase(const qualified_key& key);

    /** @brief Makes a deep copy of a module
     *
     * This function makes a deep copy of a module. The new module is unlocked
     * regardless of whether the old module was locked or not. The user can call
     * lock on the resulting module to make an exact copy
     *
     * @param old_key The module to copy
     * @param new_key The key under which the new module will live. The copy
     *                always goes in the own map.
     */
    void copy_module(const qualified_key& old_key, type::key new_key);

    /// Copies @p old_key to @p new_key, then erases @p old_key
    void rename_module(const qualified_key& old_key, type::key new_key);

    /** @brief Returns the module @p key refers to.
     *
     *  @param key The module you want
     *  @return A shared_ptr to the requested module
     *
     *  @throw std::out_of_range if @p key does not refer to a module. Strong
     *                           throw guarantee.
     *  @throw std::invalid_argument if @p key is key-only and more than one
     *                               map has the key. Strong throw guarantee.
     */
    shared_module at(const qualified_key& key) const;

    /** @brief Takes ownership of @p plugin and its modules.
     *
     *  @param[in] plugin The plugin to load.
     *
     *  @throw std::invalid_argument if @p plugin is null, has no name, or has
     *                               the same name as a plugin that is already
     *                               loaded. Strong throw guarantee for *this.
     */
    void load_plugin(plugin_pointer plugin);

    /// Applies the default submodules the loaded plugins recorded
    void wire_defaults();

    /// Is a plugin named @p name loaded?
    bool has_plugin(const name_type& name) const noexcept;

    /// The names of the maps, "" (the own map) first, then plugins in load
    /// order
    name_container_type plugin_names() const;

    /** @brief Returns the loaded plugin named @p name.
     *
     *  @throw std::out_of_range if no plugin named @p name is loaded. Strong
     *                           throw guarantee.
     */
    const Plugin& get_plugin(const name_type& name) const;

    ///@{
    /** @brief Returns the map named @p name.
     *
     *  @param[in] name The name of a loaded plugin, or "" for the own map.
     *
     *  @throw std::out_of_range if @p name is not "" and no plugin named
     *                           @p name is loaded. Strong throw guarantee.
     */
    module_map& get_modules(const name_type& name);
    const module_map& get_modules(const name_type& name) const;
    ///@}

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
    ///@}

    void set_runtime(runtime_ptr runtime) noexcept { m_runtime_ = runtime; }

    runtime_type& get_runtime() const { return *m_runtime_; }

    /// The qualified keys of every module, in plugin_names() order
    key_container_type keys() const;

    bool has_cache() const noexcept { return static_cast<bool>(m_pcaches); }

    ///@{
    /** @name ModuleManager state
     *
     *  The members in this section are the state of the ModuleManager class.
     */
    // These are the Modules in the states provided to us by the developer
    base_map m_bases;

    // These are the Modules that do not come from a plugin, in the state set
    // by the user
    module_map m_modules;

    // Part of the hacky patch to make multiple python modules work
    // TODO: remove when a more elegant solution is determined
    // These are the Python Modules in their developer state
    py_base_map m_py_bases;

    // These are the loaded plugins, in load order. Each plugin's ModuleMap
    // holds the plugin's modules in the state set by the user.
    plugin_container_type m_plugins;

    // These are the results of the modules running in the user's states
    cache_pointer m_pcaches;

    // Pointer to this modules current runtime
    runtime_ptr m_runtime_;
    ///@}
private:
    /// Returns the loaded plugin named @p name, or null if there isn't one
    Plugin* find_plugin_(const name_type& name) const noexcept;

    /// Returns the map named @p name, or null if there isn't one
    const module_map* find_map_(const name_type& name) const noexcept;

    /// Returns the name of the map holding @p key, throwing if there isn't
    /// exactly one
    name_type resolve_(const qualified_key& key) const;

    /// Gives @p mod the runtime and caches, using @p cache_key for the module
    /// cache
    void attach_(const type::key& cache_key, Module& mod) const;
};

} // namespace pluginplay::detail_

#include "module_manager_pimpl.ipp"
