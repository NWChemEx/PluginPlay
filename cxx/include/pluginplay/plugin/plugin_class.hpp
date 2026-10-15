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
#include <map>
#include <pluginplay/module_manager/module_map.hpp>
#include <string>
#include <tuple>
#include <utilities/containers/case_insensitive_map.hpp>
#include <utility>
#include <vector>

namespace pluginplay {
namespace detail_ {
struct ModuleManagerPIMPL;
}

/** @brief Base class for all plugins.
 *
 *  A plugin is a named collection of modules. Plugins are written like
 *  modules: the derived class's constructor calls the protected member
 *  functions of this class to set the plugin's name, add its modules, wire
 *  its modules together, and record which modules from other plugins it
 *  would like to use as default submodules. After construction the plugin is
 *  read-only through its public interface.
 *
 *  See the "Designing the Plugin Class" page of the developer documentation
 *  for the design.
 */
class Plugin {
public:
    /// Type used for the names of plugins
    using name_type = std::string;

    /// Type of a list of plugin names
    using provider_list_type = std::vector<name_type>;

    /// Type used for module keys
    using key_type = ModuleMap::key_type;

    /// Type of a pointer to a module's implementation
    using module_base_ptr = ModuleMap::module_base_ptr;

    /** @brief One default submodule recorded with default_submod_from.
     *
     *  If the provider is loaded, the submodule @p callback_key of this
     *  plugin's module @p module_key should be set to the provider's module
     *  @p provider_key.
     */
    struct DefaultSubmod {
        /// The key of this plugin's module whose submodule is set
        key_type module_key;

        /// The key the module uses for the submodule
        key_type callback_key;

        /// The key of the provider's module to use as the submodule
        key_type provider_key;

        /// True if all three keys equal their counterparts in @p rhs
        bool operator==(const DefaultSubmod& rhs) const {
            return std::tie(module_key, callback_key, provider_key) ==
                   std::tie(rhs.module_key, rhs.callback_key, rhs.provider_key);
        }

        /// Negates operator==
        bool operator!=(const DefaultSubmod& rhs) const {
            return !(*this == rhs);
        }
    };

    /// Type of a list of recorded default submodules
    using default_list_type = std::vector<DefaultSubmod>;

    /// Polymorphic base class, so the destructor is virtual
    virtual ~Plugin() = default;

    /// The name of the plugin, *e.g.*, "my_plugin". No throw guarantee.
    const name_type& name() const noexcept { return m_name_; }

    /** @brief The plugins to get default submodules from.
     *
     *  @return The names of the preferred providers, highest priority first.
     *
     *  @throw None No throw guarantee.
     */
    const provider_list_type& preferred_providers() const noexcept {
        return m_providers_;
    }

    /** @brief The default submodules recorded for @p provider.
     *
     *  @param[in] provider The name of the provider.
     *
     *  @return The records for @p provider, in the order they were recorded.
     *          Empty if no records were made for @p provider.
     *
     *  @throw None No throw guarantee.
     */
    const default_list_type& default_submods_from(
      const name_type& provider) const noexcept;

    /// The plugin's modules, in their current state. No throw guarantee.
    const ModuleMap& get_modules() const noexcept { return m_modules_; }

    /** @brief Determines if *this and @p rhs describe the same plugin.
     *
     *  Two plugins are equal if they have the same name, preferred providers,
     *  recorded default submodules, and modules.
     */
    bool operator==(const Plugin& rhs) const;

    /// Negates operator==
    bool operator!=(const Plugin& rhs) const { return !(*this == rhs); }

    /** @brief Determines if @p name can be used as a plugin name.
     *
     *  Plugin names must be identifiers: a letter or an underscore, followed
     *  by letters, digits, and underscores, *e.g.*, "my_plugin". Plugin names
     *  are used as directory names (see printing::document_modules) and in
     *  module cache names, so they can not contain spaces, path separators,
     *  or "::".
     *
     *  @param[in] name The name to check.
     *
     *  @return True if @p name is a valid plugin name and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    static bool is_valid_name(const name_type& name) noexcept;

protected:
    /// Plugins are only created through derived classes
    Plugin() = default;

    /** @brief Sets the name of the plugin.
     *
     *  Like module keys, plugin names are case-insensitive. The name is stored
     *  as given.
     *
     *  @param[in] name The name of the plugin. Must be an identifier (see
     *                  is_valid_name).
     *
     *  @throw std::invalid_argument if @p name is not a valid plugin name.
     *                               This includes the empty name, which is
     *                               reserved for the ModuleManager's own
     *                               modules. Strong throw guarantee.
     */
    void set_name(name_type name);

    /** @brief Adds a module with implementation @p T to the plugin.
     *
     *  @throw ??? Throws if ModuleMap::add_module throws. Same guarantee.
     */
    template<typename T>
    void add_module(key_type key) {
        m_modules_.add_module<T>(std::move(key));
    }

    /** @brief Adds a module with implementation @p base to the plugin.
     *
     *  @throw ??? Throws if ModuleMap::add_module throws. Same guarantee.
     */
    void add_module(key_type key, module_base_ptr base) {
        m_modules_.add_module(std::move(key), std::move(base));
    }

    /** @brief Links two of the plugin's own modules.
     *
     *  @throw ??? Throws if ModuleMap::change_submod throws. Same guarantee.
     */
    void change_submod(const key_type& module_key, const key_type& callback_key,
                       const key_type& submod_key) {
        m_modules_.change_submod(module_key, callback_key, submod_key);
    }

    /** @brief Sets the plugins to get default submodules from.
     *
     *  @tparam Args The types of the names. Must be convertible to name_type.
     *
     *  @param[in] names The names of the providers, highest priority first.
     *
     *  @throw ??? Throws if the non-template overload throws. Same guarantee.
     */
    template<typename... Args>
    void set_preferred_providers(Args&&... names) {
        set_preferred_providers(
          provider_list_type{name_type(std::forward<Args>(names))...});
    }

    /** @brief Sets the plugins to get default submodules from.
     *
     *  Replaces any previously set preferred providers.
     *
     *  @param[in] names The names of the providers, highest priority first.
     *
     *  @throw std::invalid_argument if a name is not a valid plugin name (see
     *                               is_valid_name), if a name appears more
     *                               than once, or if a provider that already
     *                               has recorded defaults is not in @p names.
     *                               Strong throw guarantee.
     */
    void set_preferred_providers(provider_list_type names);

    /** @brief Records a default submodule from another plugin.
     *
     *  If @p provider is loaded, wiring sets the submodule @p callback_key of
     *  this plugin's module @p module_key to the provider's module
     *  @p provider_key, unless the submodule is already set.
     *
     *  @param[in] provider The name of the plugin providing the module.
     *  @param[in] module_key The key of this plugin's module.
     *  @param[in] callback_key The key @p module_key uses for the submodule.
     *  @param[in] provider_key The key of the provider's module.
     *
     *  @throw std::invalid_argument if @p provider is not a preferred
     *                               provider. Strong throw guarantee.
     *  @throw std::out_of_range if @p module_key is not one of this plugin's
     *                           modules, or if @p callback_key is not a
     *                           submodule of @p module_key. Strong throw
     *                           guarantee.
     */
    void default_submod_from(const name_type& provider, key_type module_key,
                             key_type callback_key, key_type provider_key);

    /// The plugin's modules, for passing to helper functions
    ModuleMap& modules() noexcept { return m_modules_; }

private:
    /// The ModuleManager owns loaded plugins and changes their modules
    friend struct detail_::ModuleManagerPIMPL;

    /// The name of the plugin
    name_type m_name_;

    /// The preferred providers, highest priority first
    provider_list_type m_providers_;

    /// The recorded default submodules, keyed by provider
    utilities::CaseInsensitiveMap<default_list_type> m_defaults_;

    /// The plugin's modules
    ModuleMap m_modules_;
};

} // namespace pluginplay
