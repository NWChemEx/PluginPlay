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
