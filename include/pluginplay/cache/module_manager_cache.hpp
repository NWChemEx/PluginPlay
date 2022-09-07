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
#include <string>

namespace pluginplay::cache {
namespace detail_ {
class ModuleManagerCachePIMPL;
}

class ModuleCache;
class UserCache;

/** @brief "The Cache". This object holds all of the data the ModuleManager
 *         needs to cache.
 *
 *  For convenience PluginPlay defines a single cache object. This cache object
 *  encapsulates all of the data that needs to be saved/loaded for whatever
 *  reason. The ModuleManagerCache is this single cache. Each ModuleManager
 *  instance has one ModuleManagerCache internally.
 *
 *  Notes on parallel behavior:
 *
 *  This class is relatively ignorant of parallelism. What this means is that
 *  much of its internals do not do anything special in a parallel enviornment;
 *  however, this should be fine. In particular:
 *
 *  - the cache always uses process local memory (synchrnoization can be added
 *    later, but most initial usecases have SIMD interactions with modules)
 *  - whether each process writes to a local disk or a common location is
 *    determined by the directory path given to the instance. In general it is
 *    the user's responsibility to choose whether all the processes see the
 *    same cache (for example by providing a path on a parallel filesystem), or
 *    if there multiple caches (for example by providing paths that are only
 *    visible to a proper subset of processes).
 *  - in most cases functions are not thread-safe. For memoization this may lead
 *    to cache-misses on account of data races (e.g., thread 1 is computing, but
 *    hasn't cached a result that thread 2 is looking for. The result is thread
 *    2 will duplicate the effort, but otherwise there's no harm done).
 */
class ModuleManagerCache {
public:
    /// Type of the object used as keys for module (and user) caches
    using module_cache_key = std::string;

    /// Type of object users should use to specify directory paths
    using path_type = std::string;

    /// Type of the per-module cache, this cache is used for memoization
    using module_cache_type = ModuleCache;

    /// Type of a shared pointer to a per-module cache
    using module_cache_pointer = std::shared_ptr<module_cache_type>;

    /// Type of the cache that module developers can store state in
    using user_cache_type = UserCache;

    /// Type of a shared pointer to a user_cache_type object
    using user_cache_pointer = std::shared_ptr<user_cache_type>;

    /** @brief Creates a new instance that does not save to disk.
     *
     *  Default created ModuleManagerCache instances have no state and will
     *  store their cached results in memory, with no option of backing up to
     *  disk. Users can enable disk backups by calling change_save_location on
     *  a ModuleManagerCache instance. .
     */
    ModuleManagerCache() noexcept;

    /** @brief Creates a new instance which does save to disk.
     *
     *  This ctor is used for both loading existing caches from disk and
     *  creating new caches which can save to disk. If @p disk_location points
     *  to a directory that does not exist, that directory will be created and
     *  used for saving the cache (the cache actually consists of multiple files
     *  which is why it is a directory and not a file). If @p disk_location
     *  already exists then we assume you want us to reuse the cache that
     *  already lives at @p disk_location.
     *
     *
     *  @param[in] disk_location The (ideally full) path to the directory where
     *                           cached results will be saved.
     *
     */
    explicit ModuleManagerCache(path_type disk_location);

    /** @brief Default dtor.
     *
     *  This desctructor simply cleans up the memory. In particular it does not
     *  force a save or anything like that.
     *
     *  @throw None No throw guarantee.
     */
    ~ModuleManagerCache() noexcept;

    /** @brief Changes/sets the disk location where the cache is backed up to.
     *
     *  If you want the cache to be saved to disk it is strongly advised you
     *  use the ctor which takes a path. That said, you can use this method to
     *  make it such that future get_or_make calls create/load caches from a
     *  disk-based location. In particular, this will not affect caches which
     *  have been created by this instance prior to calling change_save_location
     *  (i.e., if you made some in memory-only caches, this method will not
     *  reallocate them/migrate their data to a cache which can save to disk).
     *
     *  @param[in] disk_location Where the cache should be saved to.
     */
    void change_save_location(path_type disk_location);

    /** @brief Retrieves the module cache for @p key.
     *
     *  For module implementations which can be memoized, the cache holds a
     *  module cache instance for this purpose. That cache is shared by all
     *  instances of that implementation. It is the caller's responsibility to
     *  assign unique identifiers to each different module implementation.
     *
     *  This function is used to retrieve the module cache for the module
     *  associated with @p key. If the cache does not exist prior to this call
     *  it will be created and then returned.
     *
     *  @param[in] key The identifier for the specific module implementation
     *                 whose module cache we are to retrieve.
     *
     *  @return A pointer to the requested module cache.
     *
     *  @throw std::bad_alloc if the cache does not already exist and there is a
     *                        problem allocating it.
     */
    module_cache_pointer get_or_make_module_cache(module_cache_key key);

    /** @brief Retrieves the requested user cache.
     *
     *  Module developers may need to cache internal state. To do this
     *  PluginPlay associates with each module implementation a user cache. Like
     *  the module cache, the user cache is shared by all isntances of the
     *  same algorithm.
     *
     *  This method retrieves the user cache associated with @p key. If the
     *  cache does not exist prior to this call it will be created and then
     *  returned.
     *
     *  @param[in] key The identifier for the specific module implementation
     *                 whose user cache we are to retrieve.
     *
     *  @return A pointer to the requested user cache.
     *
     *  @throw std::bad_alloc if the cache does not already exist and there is a
     *         problem allocating it. Strong throw guarantee.
     */
    user_cache_pointer get_or_make_user_cache(module_cache_key key);

private:
    /// Type of the object actually implementing this class
    using pimpl_type = detail_::ModuleManagerCachePIMPL;

    /// Type of read/write reference to the PIMPL
    using pimpl_reference = pimpl_type&;

    /// Type of read-only reference to the PIMPL
    using const_pimpl_reference = const pimpl_type&;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Code factorization for making a moduel cache
    module_cache_type make_module_cache_(module_cache_key key);

    /// Asserts that the object has a PIMPL, throws std::runtime_error if not
    void assert_pimpl_() const;

    /// Returns PIMPL in read/write state, making it if it DNE
    pimpl_reference pimpl_();

    /// Returns PIMPL in read-only state after asserting it exists
    const_pimpl_reference pimpl_() const;

    /// The actual PIMPL
    pimpl_pointer m_pimpl_;
};

} // namespace pluginplay::cache
