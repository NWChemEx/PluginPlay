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
     *  N.B. Only one directory will be created. So if you give a path `/A/B/C`
     *  the path `/A/B` must already exist as this function will only create the
     *  `C` directory. `C` is also only created if `/A/B/C` is not already a
     *  directory (if it already exists then we assume you are trying to load
     *  from `/A/B/C`).
     *
     *  @param[in] disk_location The (ideally full) path to the directory where
     *                           cached results will be saved.
     *
     */
    explicit ModuleManagerCache(path_type disk_location);

    ~ModuleManagerCache() noexcept;

    void change_save_location(path_type disk_location);

    module_cache_pointer get_or_make_module_cache(module_cache_key key);

    user_cache_pointer get_or_make_user_cache(module_cache_key key);

private:
    /// Type of the object actually implementing this class
    using pimpl_type = detail_::ModuleManagerCachePIMPL;

    /// Type of read/write reference to the PIMPL
    using pimpl_reference = pimpl_type&;

    /// Type of read-only reference to the PIMPL
    using const_pimpl_reference = const pimpl_type&;

    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    module_cache_type make_module_cache_(module_cache_key key);

    void assert_pimpl_() const;

    pimpl_reference pimpl_();

    const_pimpl_reference pimpl_() const;

    pimpl_pointer m_pimpl_;
};

} // namespace pluginplay::cache
