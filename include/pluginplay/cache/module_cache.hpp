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
#include <pluginplay/cache/module_manager_cache.hpp>
#include <pluginplay/fields/fields.hpp>
#include <pluginplay/types.hpp>

namespace pluginplay::cache {
namespace detail_ {
class ModuleCachePIMPL;
}

/** @brief Class storing the memoized results for a particular module
 *         implementation.
 *
 *  Each module implementation (as determined by the class derived from
 *  ModuleBase) has a ModuleCache instance assigned to it. These module caches
 *  are where calls to the module get memoized to. Instances of ModuleCache
 *  behave like a map from input maps to result maps.
 *
 */
class ModuleCache {
public:
    /// Type of the object used to uniquely identify a module
    using module_uuid_type = typename ModuleManagerCache::module_cache_key;

    /// Type of the keys, semantically similar to std::map<string, ModuleInput>
    using key_type = type::input_map;

    /// Read-only reference to a key
    using const_key_reference = const key_type&;

    /// Type of results, semantically similar to std::map<string, ModuleResult>
    using mapped_type = type::result_map;

    /// Type of the object holding the ModuleCache's state
    using pimpl_type = detail_::ModuleCachePIMPL;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /** @brief Creates a ModuleCache instance with no state.
     *
     *  In a normal workflow ModuleCache instances are created by
     *  ModuleManagerCache instances. The default ctor allows you to create
     *  a ModuleCache that has no state (specifically no PIMPL). The resulting
     *  instance is largely a placeholder, as attempting to cache/uncache will
     *  raise errors.
     *
     *  @throw None No throw guarantee.
     */
    ModuleCache() noexcept;

    /** @brief Creates a ModuleCache instance with the provided state.
     *
     *  The definition of the PIMPL class is not publically accessible so users
     *  are not expected to use this ctor. This is the ctor that
     *  ModuleManagerCache instances use to create ModuleCache instances.
     *
     *  @param[in] pimpl The instance containing the state of the ModuleCache.
     *                   Under normal circumstances @p pimpl will be created by
     *                   a ModuleManagerCache instance.
     *
     *  @throw None No throw guarantee.
     */
    explicit ModuleCache(pimpl_pointer pimpl) noexcept;

    /** @brief Creates a ModuleCache instance by taking the state from an
     *         already existing instance.
     *
     *
     *  @param[in,out] other The instance we are taking the state from. After
     *                       the call @p other will be in a state consistent
     *                       with being default initialized.
     *
     *  @throw None No throw guarantee.
     */
    ModuleCache(ModuleCache&& other) noexcept;

    /** @brief Standard default dtor.
     *
     *  The destructor simply frees up memory associated with the Cache's state.
     *  In particular it does not attempt to dump the instance's state to disk.
     *
     *  @throw None No throw guarantee.
     */
    ~ModuleCache() noexcept;

    /** @brief Determines if there is a memoized result under @p key.
     *
     *  Each call to a module is viewed as a mapping from inputs (including
     *  UUIDs of any submodules) to results. This method allows the caller to
     *  determine if the module has already been called with the inputs @p key.
     *  It is the caller's responsibility to make sure @p key includes all of
     *  information that can possibly influence the result.
     *
     *  N.B. If this instance does not have a PIMPL this function will always
     *       return false.
     *
     *  @param[in] key The inputs to look up.
     *
     *  @return True if this cache already contains results for @p key and false
     *          otherwise.
     *
     *  @throw ??? Throws if the database backend throws. Same throw guarantee.
     */
    bool count(const_key_reference key) const;

    /** @brief Stores the provided key/value pair in the cache.
     *
     *  After a module has been run we memoize the result. This method will
     *  use the inputs to the module as a key and the results from the module as
     *  the associated value. It is the caller's responsiblity to ensure that
     *  @p key includes all information that can possibly influence the result.
     *
     *  @param[in] key The inputs which generated @p value.
     *
     *  @param[in] value The results generated by running the module with the
     *                   inputs in @p key.
     *
     *  @throw std::out_of_range if this instance does not contain a backend or
     *                           if there are no results cached under @p key.
     *                           Strong throw guarantee.
     *  @throw ??? If the backend throws. Strong throw guarantee.
     */
    void cache(key_type key, mapped_type value);

    /** @brief Retrieves previously cached results.
     *
     *  This method is used to retrieve the results which were generated with
     *  the inputs @p key. It is the user's responsiblity to make sure that
     *  @p key contains enough information to avoid a cache collision (i.e.,
     *  this class makes no attempt to ensure that the returned results should
     *  be used for memoization).
     *
     *  @param[in] key The inputs associated with the results we want.
     *
     *  @return The results which were cached under @p key.
     *
     *  @throw std::runtime_error if this instance does not contain a PIMPL.
     *                            Strong throw guarantee.
     *
     */
    mapped_type uncache(const_key_reference key);

    /** @brief Frees up the memory associated with this cache.
     *
     *  @warning This function will delete all results and will not save them.
     *
     *  Calling this method will delete all of the results in the cache and will
     *  make no attempt to save the results to long-term storage.
     *
     *  N.B. This is a no-op if this instance does not contain a PIMPL.
     *
     *  @throw ??? Throws if the backend throws.
     */
    void clear();

private:
    /// Type of a modifiable PIMPL
    using pimpl_reference = pimpl_type&;

    /// Type of a reference to a read-only PIMPL
    using const_pimpl_reference = const pimpl_type&;

    /// Raises std::runtime_error if m_pimpl_ is null
    void assert_pimpl_() const;

    /// Asserts we have a PIMPL then returns it
    pimpl_reference pimpl_();

    /// Asserts we have a PIMPL then returns it (read-only)
    const_pimpl_reference pimpl_() const;

    /// The object holding the state of the object
    pimpl_pointer m_pimpl_;
};

} // namespace pluginplay::cache
