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
#include <pluginplay/cache/module_cache.hpp>

namespace pluginplay::cache {

/** @brief A place for users to store intermediate module artifacts.
 *
 *  Sometimes users need to store results betweeen calls to a module. For
 *  example many iterative modules may require knowledge of prior guesses in
 *  order to produce a new guess. UserCache objects are a place where users can
 *  put these intermediate results and still have them be part of the broader
 *  cache. Users need to be careful to tag stored results in a manner that ties
 *  them back to the inputs that generated them as subsequent calls to the
 *  module are not necessarilly continuing the same calculation.
 *
 *  As an implementation note UserCache instances just wrap ModuleCache
 *  instances (although they are distict ModuleCache instance from the one the
 *  module uses for memoization). The UserCache simply type-erases the inputs
 *  and results its given and feeds them into the underlying ModuleCache.
 */
class UserCache {
public:
    /// Type used to implement the UserCache
    using sub_cache_type = ModuleCache;

    /** @brief Creates a default initialized UserCache.
     *
     *  Default constructed UserCache instances contain default initialized
     *  state and thus can not actually be used for cacheing/uncacheing. In
     *  practice users get UserCache instances from an existing
     *  ModuleManagerCache instance and the default ctor serves primarily as a
     *  means of creating placeholder instances.
     *
     *  @throw None No throw guarantee.
     */
    UserCache() = default;

    /** @brief Creates a UserCache that uses @p cache for the implementation
     *
     *  UserCache instances are typically created by ModuleManagerCache
     *  instances. This is the ctor that the ModuleManagerCache instance uses to
     *  create the UserCache.
     *
     *  @param[in] cache A ModuleCache instance. The resulting UserCache will
     *                   be implemented by wrapping the provided ModuleCache.
     *
     *  @throw None No throw guarantee.
     */
    explicit UserCache(sub_cache_type cache) : m_cache_(std::move(cache)) {}

    /** @brief Determines if @p key appears in the cache or not.
     *
     *  Module developers are allowed to cache their module's state in the
     *  provided UserCache. When they do this it's through a key/value API.
     *  This method allows the caller to determine if a value has been cached
     *  under the provided key.
     *
     *  N.B. The actual look up of the key is done in a type-erased manner that
     *       is slightly sensitive to the type of @p key.
     *
     *
     *  @tparam T The type of @p key. @p T will be determined by the compiler.
     *            In rare cases the caller may want to specify @p T to ensure
     *            @p key gets passed as the correct type. For example string
     *            literals are not the same thing as `std::string`. @p T must be
     *            type-erasable to an AnyField.
     *
     *  @return True if there are results cached under @p key and false
     *          otherwise.
     */
    template<typename T>
    bool count(T&& key) const;

    /** @brief Adds the provided key/value pair to the cache.
     *
     *  This method is used to store a key/value pair in the user cache. Callers
     *  should be aware that the next call to their module is not necessarilly
     *  continuing the calculation and sshould choose @p key appropriately so as
     *  to avoid collisions.
     *
     *  N.B. Callers can use std::tuple to use multiple objects as a key or a
     *       value.
     *
     *  @tparam T The type of @p key. Must be type-erasable to an AnyField.
     *  @tparam U The type of @p value. Must be type-erasable to an AnyField.
     *
     *  @param[in] key The object used to tag the result. It is the caller's
     *                 responsibilty to make sure @p key is unique to the inputs
     *                 which generated the result.
     *  @param[in] value The object to associate with @p key.
     *
     *  @throw std::runtime_error if the wrapped ModuleCache is default
     *                            initialized. Strong throw guarantee.
     */
    template<typename T, typename U>
    void cache(T&& key, U&& value);

    /** @brief Retrieves a cached value.
     *
     *  This method is used to reverse the cache call. On account of the type-
     *  erasure used to insert the value, this method requires the caller to
     *  specify the type of the object they want to get back.
     *
     *  @tparam U The type of the object that was cached under @p key. This
     *            parameter must be specified by the caller.
     *  @tparam T The type of @p key. @p T must be type-erasable to an AnyField.
     *
     *  @param[in] key The object which was associated with the value we want.
     *
     *  @return The value that was cached under @p key.
     *
     *  @throw std::runtime_error if there is a value cached under @p key, but
     *                            it can not be implicitly converted to type
     *                            @p U.
     *  @throw std::out_of_range if there is no value cached under @p key or if
     *                           this instance contains a default initialized
     *                           ModuleCache. Strong throw guarantee.
     */
    template<typename U, typename T>
    U uncache(T&& key);

    /** @brief Uncache with optional default value.
     *
     *  This method checks to see if there's a result cached under @p key. If so
     *  that result is returned. If not, then @p default_value is returned.
     *  This is largely intended as code factorization for the very common use
     *  case when you attempt to restart from a cached result, but need to use
     *  a default value if no cached result exists.
     *
     *  @tparam U The type of the result. Results are stored in a type-erased
     *            manner so you must know the result's type to uncache it.
     *  @tparam T The type of the key.
     *  @tparam V The type of the default argument. Must be implicitly
     *            convertible to type U.
     *
     *  @param[in] key The object used to store the result.
     *  @param[in] default_value The value to return if there is no result
     *                           stored under @p key.
     *
     *  @return The result stored under @p key (if there is one) otherwise
     *          @p default_value is returned.
     *
     */
    template<typename U, typename T, typename V>
    U uncache(T&& key, V&& default_value);

    /** @brief Deletes the contents of the cache.
     *
     *  @warning Calling this member will delete the cached results. No attempt
     *           at long-term archival of the results will be made.
     *
     *  This method is used to clear out the current cache. More specifically
     *  calling this method will release all memory held by the already cached
     *  entries. No attempt will be made to move the cached entries to a long-
     *  term archival medium before the clear is done.
     *
     *  @throw ??? Throws if the backend throws. Same throw guarantee.
     */
    void reset_cache() { m_cache_.clear(); }

private:
    /// Type of the keys in the wrapped ModuleCache
    using input_map_type = typename sub_cache_type::key_type;

    /// Type of the values in the wrapped ModuleCache
    using result_map_type = typename sub_cache_type::mapped_type;

    /// Wraps going from an object of type @p T to an input_map_type
    template<typename T>
    input_map_type wrap_inputs_(T&& key) const;

    /// Wraps going from an object of type @p T to a result_map_type
    template<typename T>
    result_map_type wrap_results_(T&& value) const;

    /// Wraps going from a result_map_type to an object of type T
    template<typename T>
    T unwrap_results_(result_map_type value) const;

    /// The object actually implementing the UserCache
    sub_cache_type m_cache_;
};

} // namespace pluginplay::cache

#include "user_cache.ipp"
