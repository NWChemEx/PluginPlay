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
#include <memory>
#include <pluginplay/module/module_base.hpp>
#include <pluginplay/module/module_class.hpp>
#include <pluginplay/types.hpp>
#include <utility>
#include <vector>

namespace pluginplay {

/** @brief A container of modules, indexed by module key.
 *
 *  ModuleMap is a thin wrapper around a case-insensitive map from module key
 *  to module. It decouples creating modules from storing them in a
 *  ModuleManager, *e.g.*, a Plugin builds a ModuleMap in its constructor
 *  without needing a ModuleManager.
 *
 *  Copies of a ModuleMap are shallow, *i.e.*, the copy and the original
 *  share the same Module objects.
 */
class ModuleMap {
public:
    /// Type used for module keys
    using key_type = type::key;

    /// Type of a pointer to a usable module
    using module_pointer = std::shared_ptr<Module>;

    /// Type of a pointer to a module's implementation
    using module_base_ptr = std::shared_ptr<ModuleBase>;

    /// Type of the container *this wraps
    using map_type = utilities::CaseInsensitiveMap<module_pointer>;

    /// Type of a read/write iterator over the key/module pairs
    using iterator = typename map_type::iterator;

    /// Type of a read-only iterator over the key/module pairs
    using const_iterator = typename map_type::const_iterator;

    /// Type of a container holding the module keys
    using key_container_type = std::vector<key_type>;

    /** @brief Creates a module with the default-constructed implementation
     *         @p T and stores it under @p key.
     *
     *  @tparam T The type of the module implementation. Must derive from
     *            ModuleBase and be default constructible.
     *
     *  @param[in] key The key to store the module under.
     *
     *  @throw std::invalid_argument if @p key is already in use. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the module.
     *                        Strong throw guarantee.
     */
    template<typename T>
    void add_module(key_type key) {
        add_module(std::move(key), std::make_shared<T>());
    }

    /** @brief Creates a module from the implementation @p base and stores it
     *         under @p key.
     *
     *  @param[in] key The key to store the module under.
     *  @param[in] base The implementation of the module.
     *
     *  @throw std::invalid_argument if @p key is already in use or if @p base
     *                               is null. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the module.
     *                        Strong throw guarantee.
     */
    void add_module(key_type key, module_base_ptr base);

    /** @brief Stores the already-created module @p module under @p key.
     *
     *  This overload is for adding a module that already exists, *e.g.*, a
     *  copy of a module. The module's name is set to @p key.
     *
     *  @param[in] key The key to store the module under.
     *  @param[in] module The module to store.
     *
     *  @throw std::invalid_argument if @p key is already in use. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the module.
     *                        Strong throw guarantee.
     */
    void add_module(key_type key, Module module);

    /** @brief Stores an unlocked copy of the module @p old_key under
     *         @p new_key.
     *
     *  The copy has its own inputs and submodules, so changing them does not
     *  change the module stored under @p old_key. The copy shares the
     *  implementation of @p old_key.
     *
     *  @param[in] old_key The key of the module to copy.
     *  @param[in] new_key The key to store the copy under.
     *
     *  @throw std::out_of_range if *this does not have a module with key
     *                           @p old_key. Strong throw guarantee.
     *  @throw std::invalid_argument if @p new_key is already in use. Strong
     *                               throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the copy. Strong
     *                        throw guarantee.
     */
    void copy_module(const key_type& old_key, key_type new_key);

    /** @brief Removes the module stored under @p key.
     *
     *  Modules that use the removed module as a submodule keep using it. This
     *  function is a no-op if *this does not have a module with key @p key.
     *
     *  @param[in] key The key of the module to remove.
     *
     *  @throw None No throw guarantee.
     */
    void erase(const key_type& key) noexcept { m_modules_.erase(key); }

    /** @brief Changes the value of an input of the module @p key.
     *
     *  @tparam T The type of the new value.
     *
     *  @param[in] key The key of the module whose input is being changed.
     *  @param[in] option The key of the input to change.
     *  @param[in] new_value The new value of the input.
     *
     *  @throw std::out_of_range if *this does not have a module with key
     *                           @p key, or if that module has no input
     *                           @p option. Strong throw guarantee.
     *  @throw ??? Throws if Module::change_input throws. Same guarantee.
     */
    template<typename T>
    void change_input(const key_type& key, const key_type& option,
                      T&& new_value) {
        at(key)->change_input(option, std::forward<T>(new_value));
    }

    /** @brief Determines if *this contains a module with key @p key.
     *
     *  @param[in] key The key to look for. Comparisons are case-insensitive.
     *
     *  @return 1 if *this has a module with key @p key and 0 otherwise.
     *
     *  @throw None No throw guarantee.
     */
    type::size count(const key_type& key) const noexcept {
        return m_modules_.count(key);
    }

    /// The number of modules in *this. No throw guarantee.
    type::size size() const noexcept { return m_modules_.size(); }

    /// True if *this has no modules and false otherwise. No throw guarantee.
    bool empty() const noexcept { return m_modules_.empty(); }

    /** @brief Returns the module stored under @p key.
     *
     *  @param[in] key The key of the module to retrieve.
     *
     *  @return A pointer to the requested module.
     *
     *  @throw std::out_of_range if *this does not have a module with key
     *                           @p key. Strong throw guarantee.
     */
    module_pointer at(const key_type& key) const;

    /// Returns the keys of the modules in *this
    key_container_type keys() const;

    ///@{
    /// Iterators over the key/module pairs in *this
    iterator begin() noexcept { return m_modules_.begin(); }
    const_iterator begin() const noexcept { return m_modules_.begin(); }
    iterator end() noexcept { return m_modules_.end(); }
    const_iterator end() const noexcept { return m_modules_.end(); }
    ///@}

    /** @brief Sets a submodule of one module in *this to another module in
     *         *this.
     *
     *  @param[in] module_key The key of the module whose submodule is being
     *                        set.
     *  @param[in] callback_key The key @p module_key uses for the submodule.
     *  @param[in] submod_key The key of the module in *this to use as the
     *                        submodule.
     *
     *  @throw std::out_of_range if @p module_key or @p submod_key are not in
     *                           *this, or if @p callback_key is not a
     *                           submodule of @p module_key. Strong throw
     *                           guarantee.
     *  @throw std::runtime_error if the module @p module_key is locked, or if
     *                            the new submodule does not satisfy the
     *                            property type @p callback_key requires.
     *                            Strong throw guarantee.
     */
    void change_submod(const key_type& module_key, const key_type& callback_key,
                       const key_type& submod_key);

    /** @brief Sets a submodule of a module in *this to @p submod.
     *
     *  This overload is for setting a submodule to a module from another
     *  ModuleMap.
     *
     *  @param[in] module_key The key of the module whose submodule is being
     *                        set.
     *  @param[in] callback_key The key @p module_key uses for the submodule.
     *  @param[in] submod The module to use as the submodule.
     *
     *  @throw std::out_of_range if @p module_key is not in *this, or if
     *                           @p callback_key is not a submodule of
     *                           @p module_key. Strong throw guarantee.
     *  @throw std::runtime_error if the module @p module_key is locked, or if
     *                            the new submodule does not satisfy the
     *                            property type @p callback_key requires.
     *                            Strong throw guarantee.
     */
    void change_submod(const key_type& module_key, const key_type& callback_key,
                       module_pointer submod);

    /** @brief Determines if a submodule of a module in *this has been set.
     *
     *  @param[in] module_key The key of the module to check.
     *  @param[in] callback_key The key @p module_key uses for the submodule.
     *
     *  @return True if the submodule has been set and false otherwise.
     *
     *  @throw std::out_of_range if @p module_key is not in *this, or if
     *                           @p callback_key is not a submodule of
     *                           @p module_key. Strong throw guarantee.
     */
    bool has_submod(const key_type& module_key,
                    const key_type& callback_key) const;

    ///@{
    /** @brief Sets a submodule, but only if it has not been set yet.
     *
     *  These overloads behave like the corresponding change_submod overloads,
     *  except that they do nothing if the submodule has already been set.
     *
     *  @return True if the submodule was set and false if it was already set.
     *
     *  @throw ??? Throws if the corresponding change_submod throws. Same
     *             guarantee.
     */
    bool default_submod(const key_type& module_key,
                        const key_type& callback_key,
                        const key_type& submod_key);
    bool default_submod(const key_type& module_key,
                        const key_type& callback_key, module_pointer submod);
    ///@}

    /** @brief Determines if *this and @p rhs hold the same modules.
     *
     *  Two ModuleMaps are equal if they have the same keys and the modules
     *  stored under each key compare equal.
     *
     *  @param[in] rhs The ModuleMap to compare to.
     *
     *  @return True if *this and @p rhs are equal and false otherwise.
     */
    bool operator==(const ModuleMap& rhs) const;

    /// Negates operator==
    bool operator!=(const ModuleMap& rhs) const { return !(*this == rhs); }

private:
    /// The modules in *this
    map_type m_modules_;
};

} // namespace pluginplay
