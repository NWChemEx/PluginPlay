#pragma once
#include <pluginplay/cache/module_cache.hpp>

namespace pluginplay::cache::detail_ {

/** @brief The class containing a ModuleCache instance's state.
 *
 *  This is just a thin-wrapper around a database. The PIMPL nature keeps the
 *  details of the database out of the public API.
 */
struct ModuleCachePIMPL {
    // Type of the class this PIMPL implements
    using parent_type = ModuleCache;

    // Type of object used as keys to parent_type
    using key_type = typename parent_type::key_type;

    // Type of object used as values to parent_type
    using mapped_type = typename parent_type::mapped_type;

    // DatabaseAPI an object must satisfy for us to be able to use it
    using db_type = database::DatabaseAPI<key_type, mapped_type>;

    // Pointer to the DB
    using db_pointer_type = std::unique_ptr<db_type>;

    // The database actually powering the ModuleCache
    db_pointer_type m_db;
};

} // namespace pluginplay::cache::detail_
