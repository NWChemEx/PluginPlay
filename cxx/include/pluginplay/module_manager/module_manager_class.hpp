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
#include <memory>
#include <parallelzone/runtime/runtime_view.hpp>
#include <pluginplay/cache/module_manager_cache.hpp>
#include <pluginplay/module/module_base.hpp>
#include <pluginplay/module/module_class.hpp>
#include <pluginplay/module_manager/module_map.hpp>
#include <pluginplay/module_manager/qualified_key.hpp>
#include <pluginplay/plugin/plugin_class.hpp>
#include <pluginplay/types.hpp>
#include <type_traits>
#include <vector>

namespace pluginplay {

namespace detail_ {
struct ModuleManagerPIMPL;
}

/** @brief Class responsible for holding and managing modules.
 *
 *  Modules are loaded into a ModuleManager in two ways: as part of a plugin
 *  (see load_plugin), or one at a time (see add_module). The ModuleManager
 *  keeps one ModuleMap per loaded plugin, plus a ModuleMap of its own for the
 *  modules that do not come from a plugin. The empty plugin name, "", refers
 *  to the ModuleManager's own modules.
 *
 *  Plugin names and module keys are case-insensitive. Module keys are unique
 *  within a map, but two maps may use the same key.
 *  Functions that take a module accept a QualifiedKey. A plain module key
 *  works as long as exactly one map has that key; otherwise the plugin name
 *  must be given too, *e.g.*, `mm.at({"integrals", "Overlap"})`.
 */
class ModuleManager {
public:
    /// The type of a pointer to a read/write module
    using module_base_ptr = std::shared_ptr<ModuleBase>;

    /// Type used to identify modules
    using qualified_key = QualifiedKey;

    /// Type used for the names of plugins
    using name_type = Plugin::name_type;

    /// Type of a container of plugin names
    using name_container_type = std::vector<name_type>;

    /// Type of a pointer to a plugin
    using plugin_pointer = std::unique_ptr<Plugin>;

    /// The type of the runtime
    using runtime_type = parallelzone::runtime::RuntimeView;

    /// A pointer to a runtime
    using runtime_ptr = std::shared_ptr<runtime_type>;

    /// Type of module key container
    using key_container_type = std::vector<qualified_key>;

    /// Type of the cache
    using cache_type = cache::ModuleManagerCache;

    /// Type of a pointer to the cache
    using cache_pointer = std::shared_ptr<cache_type>;

    ///@{
    /** @name Ctors and assignment operators
     *
     */
    ModuleManager();
    ModuleManager(runtime_ptr runtime,
                  cache_pointer cache = std::make_shared<cache_type>());
    // ModuleManager(const ModuleManager& rhs);
    // ModuleManager& operator=(const ModuleManager& rhs) {
    //     return *this = std::move(ModuleManager(rhs));
    // }
    ModuleManager(ModuleManager&& rhs) noexcept;
    ModuleManager& operator=(ModuleManager&& rhs) noexcept;
    ///@}

    /// Standard dtor
    ~ModuleManager() noexcept;

    /** @brief Determines how many modules @p key refers to.
     *
     *  @param[in] key The module to look for. If @p key is key-only, all maps
     *                 are searched.
     *
     *  @return For a key-only @p key, the number of maps that have the key.
     *          For a qualified @p key, 1 if the plugin is loaded and has the
     *          module and 0 otherwise.
     *
     *  @throw None No throw guarantee.
     */
    type::size count(const qualified_key& key) const noexcept;

    /** @brief Gets the number of modules loaded.
     *
     * @return Number of modules loaded, summed over all maps.
     */
    type::size size() const noexcept;

    /** @brief Creates a plugin of type @p PluginType and loads it.
     *
     *  @tparam PluginType The type of the plugin. Must derive from Plugin and
     *                     be default constructible.
     *
     *  @throw std::invalid_argument if a plugin with the same name is already
     *                               loaded. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the plugin. Weak
     *                        throw guarantee.
     */
    template<typename PluginType>
    void load_plugin();

    /** @brief Loads @p plugin.
     *
     *  *this takes ownership of @p plugin and its modules. Nothing is copied:
     *  the plugin's ModuleMap is the map *this uses for the plugin's modules.
     *  Each module is given *this's runtime and caches. A plugin's modules use
     *  the module cache named `"<plugin name>::<module key>"`. Modules that
     *  are reachable through submodules, but no longer in the plugin's map
     *  (*e.g.*, the plugin erased them after using them as submodules), are
     *  given the runtime and caches too, using the key they were last stored
     *  under.
     *
     *  Loading does not set default submodules between plugins; call
     *  wire_defaults after all plugins are loaded.
     *
     *  @param[in] plugin The plugin to load. If this function throws, the
     *                    plugin is destroyed.
     *
     *  @throw std::invalid_argument if @p plugin is null, has no name, or has
     *                               the same name as a loaded plugin. Strong
     *                               throw guarantee for *this.
     *  @throw std::bad_alloc if there is a problem allocating the caches. Weak
     *                        throw guarantee.
     */
    void load_plugin(plugin_pointer plugin);

    /** @brief Sets default submodules between the loaded plugins.
     *
     *  For each loaded plugin, in load order, and for each of its preferred
     *  providers that is loaded, in priority order, this applies the default
     *  submodules the plugin recorded for that provider (see
     *  Plugin::default_submod_from). A default only sets a submodule that is
     *  still unset, so choices made by the user, or by a provider with higher
     *  priority, are kept. Defaults that name a module the provider does not
     *  have are skipped, with a warning through the runtime's logger.
     *
     *  Calling this function again, *e.g.*, after loading another plugin,
     *  only fills submodules that are still unset.
     *
     *  @throw std::runtime_error if a provider's module does not satisfy the
     *                            property type of the submodule. Weak throw
     *                            guarantee.
     */
    void wire_defaults();

    /** @brief Is a plugin named @p name loaded?
     *
     *  @param[in] name The name of the plugin.
     *
     *  @return True if a plugin named @p name is loaded and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool has_plugin(const name_type& name) const noexcept;

    /** @brief The names of the maps in *this.
     *
     *  @return "", for *this's own modules, followed by the names of the loaded
     *          plugins in the order they were loaded.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *                        Strong throw guarantee.
     */
    name_container_type plugin_names() const;

    /** @brief Returns the loaded plugin named @p name.
     *
     *  @param[in] name The name of the plugin.
     *
     *  @return A read-only reference to the plugin.
     *
     *  @throw std::out_of_range if no plugin named @p name is loaded. Strong
     *                           throw guarantee.
     */
    const Plugin& get_plugin(const name_type& name) const;

    /** @brief Returns the modules in the map named @p name.
     *
     *  @param[in] name The name of a loaded plugin, or "" for the modules that
     *                  do not come from a plugin.
     *
     *  @return A read-only reference to the map.
     *
     *  @throw std::out_of_range if @p name is not "" and no plugin named
     *                           @p name is loaded. Strong throw guarantee.
     */
    const ModuleMap& get_modules(const name_type& name) const;

    /** @brief Used to add a new module to the list
     *
     *  The module is added to *this's own map (plugin name "").
     *
     * @tparam ModuleType The type of the module being added. This type must be
     *                    constructible with no arguments and must inherit from
     *                    ModuleBase.
     * @param module_key
     */
    template<typename ModuleType>
    void add_module(type::key module_key);

    /** @brief Used to add a new module to the list
     *
     *  The module is added to *this's own map (plugin name "").
     *
     * @param module_key
     * @param base
     */
    void add_module(type::key module_key, module_base_ptr base);

    ///@{
    /** @name Module retrievers
     *
     *  @param[in] module_key The module to retrieve.
     *
     *  @return A reference to the requested module.
     *
     *  @throw std::out_of_range if @p module_key does not refer to a module.
     *                           Strong throw guarantee.
     *  @throw std::invalid_argument if @p module_key is key-only and more than
     *                               one map has the key. Strong throw
     *                               guarantee.
     */
    Module& at(const qualified_key& module_key) {
        const auto& rv = const_cast<const ModuleManager&>(*this).at(module_key);
        return const_cast<Module&>(rv);
    }
    const Module& at(const qualified_key& module_key) const;
    ///@}

    /** @brief Makes a "deep copy" of a module
     *
     *  This function will copy the module stored under @p old_key and store the
     *  copy under @p new_key. The copy will be unlocked, with a deep copy of
     *  @p old_key 's inputs and submodules. Deep copy in this context meaning
     *  that changing @p new_key 's inputs or submodules will not affect the
     *  module stored under @p old_key.
     *
     *  The copy is always stored in *this's own map (plugin name ""), even if
     *  @p old_key is one of a plugin's modules. Plugins' maps only hold the
     *  modules the plugins provide.
     *
     *  @param[in] old_key The module which is being copied.
     *  @param[in] new_key The key for the resulting copy.
     *
     *  @throw std::out_of_range if @p old_key does not refer to a module.
     *                           Strong throw guarantee.
     *  @throw std::invalid_argument if @p old_key is ambiguous or *this's own
     *                               map already has a module with key
     *                               @p new_key. Strong throw guarantee.
     *  @throw std::bad_alloc if an allocation error arises.
     */
    void copy_module(const qualified_key& old_key, type::key new_key);

    /** @brief Unloads the specified module.
     *
     *  This function unloads the module with the specified key. After this
     *  operation the key is free to be used again. Calling this function does
     *  NOT clean any data out of the cache. This function is a no-op if @p key
     *  does not exist.
     *
     *  @param[in] key The module which should be erased.
     *
     *  @throw std::invalid_argument if @p key is key-only and more than one map
     *                               has the key. Strong throw guarantee.
     */
    void erase(const qualified_key& key);

    /** @brief Changes the key a module is stored under.
     *
     *  This function can be used to rename the key a module is stored under.
     *  After a call to this function there will be no module stored under
     *  @p old_key.
     *
     *  Like copy_module, the renamed module is stored in *this's own map, so
     *  renaming one of a plugin's modules moves it out of the plugin's map.
     *
     *  @param[in] old_key The module we are renaming.
     *  @param[in] new_key The value for the new key.
     *
     *  @throw ??? Throws if copy_module throws. Same guarantee.
     */
    void rename_module(const qualified_key& old_key, type::key new_key);

    /** @brief Changes the value of an input bound to a module
     *
     * @tparam T
     * @param key
     * @param option
     * @param new_value
     */
    template<typename T>
    void change_input(const qualified_key& key, const type::key& option,
                      T&& new_value) {
        at(key).change_input(option, std::forward<T>(new_value));
    }

    /** @brief Changes the submodule a module calls.
     *
     *  Modules can call other modules at predesignated callback points. Each of
     *  these callback points has a key associated with it. This function allows
     *  the user to change which module is called at a particular callback point
     *  by specifying the key of the module, the callback point's key, and the
     *  key for tne new submodule.
     *
     * @param[in] module_key   The module whose submodule is being changed.
     * @param[in] callback_key The name of the callback point in the module
     *                         assigned to @p module_key
     * @param[in] submod_key   The submodule to call. It may be in a different
     *                         map than @p module_key.
     *
     * @throw std::out_of_range if any of the keys do not refer to valid,
     *                          already present modules or callback points. Weak
     *                          throw guarantee.
     * @throw std::invalid_argument if @p module_key or @p submod_key is
     *                              ambiguous. Strong throw guarantee.
     */
    void change_submod(const qualified_key& module_key,
                       const type::key& callback_key,
                       const qualified_key& submod_key);

    /** @brief Runs a given module
     *
     * @tparam T
     * @tparam Args
     * @param key
     * @param args
     * @return
     */
    template<typename T, typename... Args>
    auto run_as(const qualified_key& key, Args&&... args) {
        return at(key).run_as<T>(std::forward<Args>(args)...);
    }

    /** @brief Sets the runtime of the module mananger and its modules.
     *
     *  @param[in] runtime A shared_ptr to a @p runtime_type instance with which
     *                     the module manager is currently associated.
     *
     *  @throw None No throw guarantee.
     */
    void set_runtime(runtime_ptr runtime) noexcept;

    /** @brief Provides the runtime of the module manager.
     *
     *  @return The runtime object with which the module manager is currently
     *          associated.
     *
     *  @throw std::runtime_error if there is no runtime. Strong throw
     * guarantee.
     */
    runtime_type& get_runtime() const noexcept;

    /** @brief Returns the keys of all the modules in the module manager.
     *
     * @return The qualified keys of all the modules, *this's own modules
     *         first, followed by each plugin's modules in load order.
     *
     * @throw std::bad_alloc if an allocation error arises.
     */
    key_container_type keys() const;

    /** @brief Does *this have a cache set?
     *
     *
     *  @return True if *this has a place where it can cache input/result pairs
     *          and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_cache() const noexcept;

private:
    /** @brief Does *this have a PIMPL?
     *
     *  @return False if pimpl_ is set to a null pointer and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool has_pimpl_() const noexcept;

    /// The object that actually implements the ModuleManager
    std::unique_ptr<detail_::ModuleManagerPIMPL> pimpl_;
}; // End class ModuleManager

//------------------------------------------------------------------------------
//                          Inline Implementations
//------------------------------------------------------------------------------

template<typename ModuleType>
void ModuleManager::add_module(type::key module_key) {
    add_module(std::move(module_key), std::make_shared<ModuleType>());
}

template<typename PluginType>
void ModuleManager::load_plugin() {
    static_assert(std::is_base_of_v<Plugin, PluginType>,
                  "Plugins must derive from pluginplay::Plugin");
    load_plugin(std::make_unique<PluginType>());
}

} // namespace pluginplay
