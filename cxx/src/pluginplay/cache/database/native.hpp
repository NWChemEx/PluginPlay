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
#include "database_api.hpp"
#include <map>
#include <memory>

namespace pluginplay::cache::database {

/** @brief An in memory database the preserves objects in their native form
 *
 *  Many of the database backends abstract away, or in some other way, map from
 *  the literal data being stored (e.g. compression, serialization, or UUIDs).
 *  This database backend is used when we want to keep the key/value pairs in
 *  whatever format they were given to us, and we want those key/value pairs to
 *  be readily accessible. This class also supports backing the key/value pairs
 *  up to more persistant storage by providing a subdatabase.
 *
 *  In practice this class just wraps an std::map with our DatabaseAPI API.
 *
 *  @tparam KeyType The type of the keys we are storing.
 *  @tparam ValueType The type of the values that the keys map to.
 */
template<typename KeyType, typename ValueType>
class Native : public DatabaseAPI<KeyType, ValueType> {
private:
    /// Type the class implements
    using base_type = DatabaseAPI<KeyType, ValueType>;

public:
    /// Type of container implementing this class, specialization of std::map
    using map_type = std::map<KeyType, ValueType>;

    /// Type of the keys to this database, typedef of KeyType
    using typename base_type::key_type;

    /// Ultimately typedef of DatabaseAPI::key_set_type
    using typename base_type::key_set_type;

    /// Type of a read-only reference to a key, typedef of const KeyType&
    using typename base_type::const_key_reference;

    /// Type of the mapped values, typedef of ValueType
    using typename base_type::mapped_type;

    /// Type of an object holding a read-only reference to a value, typedef of
    /// ConstValue<mapped_type>
    using typename base_type::const_mapped_reference;

    /// Type of DatabaseAPI that can be used for backup
    using backup_db_type = DatabaseAPI<key_type, mapped_type>;

    /// Type of a pointer to a backup database
    using backup_db_pointer = std::unique_ptr<backup_db_type>;

    /** @brief Creates a new instance by wrapping the provided value.
     *
     *  This ctor creates a new Native instance. By default that instance wraps
     *  a default constructed map_type object. However, users can provide an
     *  already populated instance if they want (e.g., for restarting purposes).
     *
     *  @param[in] map The object used internally to store the key/value
     *                 relationships. Defaults to a default constructed map_type
     *                 instance.
     *
     *  @param[in] backup The database where the contents of this instance will
     *                    be backed up to. If this is a nullptr then backing up
     *                    the database will be a no-op.
     *
     *  @throw None No throw guarantee
     */
    explicit Native(map_type map = {}, backup_db_pointer backup = {});

    /** @brief Creates an empty Native instance with the provided subdatabase
     *
     *  This ctor is a convenience function for calling the primary ctor with
     *  a default constructed map_type object and the backup database @p backup.
     *
     *  @param[in] backup The database where the contents of this instance will
     *                    be backed up to. If this is nullptr then backup will
     *                    be a no-op.
     *
     *  @throw None No throw guarantee.
     */
    explicit Native(backup_db_pointer backup);

    auto& map() { return m_map_; }

    const auto& map() const { return m_map_; }

protected:
    /// Puts keys in wrapped map into a key_set_type and returns it
    key_set_type keys_() const override;

    /// Calls count on the wrapped map
    bool count_(const_key_reference key) const noexcept override;

    /// Calls operator[] on the wrapped map
    void insert_(key_type key, mapped_type value) override;

    /// Calls erase on the wrapped map
    void free_(const_key_reference key) override;

    /// Calls at on the wrapped map
    const_mapped_reference at_(const_key_reference key) const override;

    /// If a backup database was set, pushes keys to it
    void backup_() override;

    /// Calls backup then clear on m_map_
    void dump_() override;

private:
    /// The key/values the user gave to us
    map_type m_map_;

    /// The DB to backup m_map_ to
    backup_db_pointer m_backup_;
};

} // namespace pluginplay::cache::database

#include "native.ipp"
