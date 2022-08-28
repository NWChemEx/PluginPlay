#pragma once
#include "../proxy_map_maker.hpp"
#include "database_api.hpp"
#include <memory>
#include <pluginplay/fields/fields.hpp>
#include <pluginplay/types.hpp>

namespace pluginplay::cache::database {

/** @brief Wraps the process of making databases for the Cache.
 *
 *  @warning It is imperative to keep the design documentation up to date with
 *           any and all modifications made to this class. This is because there
 *           database does a lot of processing steps, which are difficult to
 *           follow from code alone. So if you change this class make sure
 *           db_pimpl_design.png reflects those changes or the next person who
 *           works on this class is going to be like "What the heck?".
 *
 *  Constructing the objects which are used as databases is a rather invovlved
 *  process. This class hides that process from the rest of PluginPlay's
 *  internals by having other classes rely on this factor to make new databases.
 *
 *  Based on the current design, the Database backend of each module's cache
 *  (and also the user caches, since they just wrap module caches) contains two
 *  shared pieces:
 *
 *  1. A DB from type-erased inputs/results to UUIDs
 *  2. A DB from proxy maps to proxy maps used by all modules
 *
 *  Each factory maintains its own copies of these pointers and injects the
 *  copies it holds.
 *
 */
class DatabaseFactory {
public:
    /// Type of the input map that a module takes
    using input_map_type = type::input_map;

    /// Type of the actual inputs in the input map, tyepdef of ModuleInput
    using module_input_type = typename input_map_type::mapped_type;

    /// Type of an object which can map input maps to proxy maps
    using input_proxy_maker = ProxyMapMaker<input_map_type>;

    /// Type of the result map that a module returns
    using result_map_type = type::result_map;

    /// Type of the actual results in the result map, typedef of ModuleResult
    using module_result_type = typename result_map_type::mapped_type;

    /// Type of the database a ModuleCache instance wraps
    using module_db_type = DatabaseAPI<input_map_type, result_map_type>;

    /// Type of a pointer to a database a ModuleCache can use
    using module_db_pointer = std::unique_ptr<module_db_type>;

    /// Type of maps where inputs or results have been replaced by proxies
    using proxy_map_type = typename input_proxy_maker::mapped_type;

    /// The type used for type-erasing inputs and results
    using any_type = type::any;

    /// The type used for proxying an input/result
    using uuid_type = typename proxy_map_type::mapped_type;

    /// Type type that inputs and results get serialized to
    using binary_type = std::string;

    /// Type of the DB used for long-term storage by all module caches
    using pm_2_pm = DatabaseAPI<proxy_map_type, proxy_map_type>;

    /// Type of a pointer to a DB satisfying pm_2_pm
    using serial_pm_pointer = std::shared_ptr<pm_2_pm>;

    /// Type of the DB used for storing the object-to-proxy relationships
    using any_2_uuid = DatabaseAPI<any_type, uuid_type>;

    /// Type of a pointer to the DB satisfying any_2_uuid
    using any_2_uuid_pointer = std::shared_ptr<any_2_uuid>;

    /// Type of a DB that can map proxy maps to result maps
    using pm_2_result_map = DatabaseAPI<proxy_map_type, result_map_type>;

    /// Type of a pointer to a pm_2_result_map DB
    using pm_2_result_map_pointer = std::unique_ptr<pm_2_result_map>;

    /** @brief Creates a new DatabaseFactory which doesn't have any long-term
     *         storage.
     *
     *  The default DatabaseFactory creates databases whose results live purely
     *  in memory. Once those databases are cleaned up those results are lost.
     *  While conceptually you can make a default constructed DatabaseFactory
     *  start having long-term storage (enabling it will not retroactively apply
     *  it to databases created by it), doing so in a real run is likely to be
     *  more headache than it's worth. Hence are recommendation is to create a
     *  different DatabaseFactory instance which handles the long-term storage.
     */
    DatabaseFactory();

    /** @brief Creates a new DatabaseFactory instance where the long-term
     *         storage databases live at the provided paths.
     *
     *  This is the ctor we expect users to invoke when they want their
     *  databases to be stored long term. The resulting databases will live in
     *  memory, but be backed up to provided "disk" paths (disk in quotes as
     *  it can be any POSIX-like path and may not be a literal disk).
     *
     *  @param[in] cache_path Where the proxy map to proxy map database should
     *                        be archived to.
     *  @param[in] uuid_path Where the input to UUID (and result to UUID)
     *                       databases will be archived to.
     *
     */
    DatabaseFactory(const std::string& cache_path,
                    const std::string& uuid_path);

    /** @brief Makes the default Database backend for the specified module.
     *
     *  N.B. This class is not responsible for assigning UUIDs to modules. How
     *       that's done is up to the caller.
     *
     *  @param[in] module_uuid This method generates a database backend specific
     *                         to the module with this UUID.
     *
     */
    module_db_pointer default_module_db(uuid_type module_uuid) const;

    /** @brief Wraps the process of making a DB that can go from proxy maps to
     *         result maps.
     *
     *  This factory is capable of generating two different proxy-map to result-
     *  map databases: one with long-term archival, and one without. The type
     *  of databse returned from this method depends on whether or not
     *  long-term archival of proxy-map to proxy-map databases has been enabled.
     *
     *
     */
    pm_2_result_map_pointer pm2result_db(uuid_type module_uuid) const;

    /** @brief Allows the user to change where the proxy map to proxy map
     *         database is stored.
     *
     *  N.B. The actual database lives in a shared pointer, so changing the
     *       location will not invalidate references being used by already
     *       created databases.
     *
     *  @param[in] path Where on the filesystem the database should live.
     *
     */
    void set_serialized_pm_to_pm(const std::string& path);

    /** @brief Creates a uuid database with no long-term storage
     *
     *
     */
    void set_type_eraser_backend();

    /** @brief Allows the user to change where the long-term uuid database
     *         lives.
     *
     *  N.B. The actual database lives in a shared pointer, so changing the
     *       location will not invalidate references being used by already
     *       created databases.
     *
     *  @param[in] path Where on the filesystem the database should live.
     */
    void set_type_eraser_backend(const std::string& path);

private:
    // The common proxy map to proxy map database used by each module's cache
    serial_pm_pointer m_serial_pm_;

    // The common AnyField to UUID database
    any_2_uuid_pointer m_any2uuid_;
};

} // namespace pluginplay::cache::database
