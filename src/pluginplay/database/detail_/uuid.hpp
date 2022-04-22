// #pragma once
// #include "database_pimpl.hpp"
// #include <boost/uuid/random_generator.hpp>
// #include <boost/uuid/uuid.hpp>

// namespace pluginplay::database::detail_ {

// /** @brief Database storing objects and the UUIDs assigned to them.
//  *
//  *  This class is responsible for maintaining a relationship betweeen objects
//  of
//  *  type @p KeyType and the UUIDs assigned to them. The class is responsible
//  for
//  *  assigning a UUID to an object if one has not been assigned previously.
//  *
//  *  N.B. The backup_db_type swaps the key/uuid order as the keys will usually
//  *       be larger than the uuid (memory wise). Most key/value DBs are
//  optimized
//  *       for relatively small keys.
//  *
//  *
//  */
// template<typename KeyType>
// class UUID : DatabasePIMPL<KeyType, boost::uuids::uuid> {
// private:
//     /// Type of database this class implements
//     using base_type = DatabasePIMPL<KeyType, boost::uuids::uuid>;

// public:
//     /// Type used to model the UUID
//     using uuid_type = boost::uuids::uuid;

//     /// Type of the database that UUIDMapper will back up to
//     using backup_db_type = DatabasePIMPL<uuid_type, KeyType>;

//     /// Type of a pointer to the backup DB
//     using backup_db_pointer = std::unique_ptr<backup_db_type>;

//     /// Type of the database that UUIDMapper will store UUIDs in
//     using map_type = DatabasePIMPL<KeyType, uuid_type>;

//     /// Type of a pointer to the database storing the object-to-UUID relation
//     using map_pointer = std::unique_ptr<map_type>;

//     /// Type of keys in the database, typedef of KeyType
//     using typename base_type::key_type;

//     /// Type of a read-only reference to a key, typedef of const KeyType&
//     using typename base_type::const_key_reference;

//     /// Type of the values that the keys map to, typedef of uuid_type
//     using typename base_type::mapped_type;

//     /// Type of an object holding a read-only reference to a value, typedef
//     of
//     /// ConstValue<mapped_type>
//     using typename base_type::const_mapped_reference;

//     /** @brief Creates a new UUID instance with the specified state.
//      *
//      *  @param[in] map The Database that UUID will use to store the key to
//      UUID
//      *                 relationship.
//      *  @param[in] backup The subdatabase that UUID will backup to.
//      *
//      */
//     UUID(map_pointer map, backup_db_pointer backup) noexcept;

// protected:
//     bool count_(const_key_reference key) const noexcept override;

//     void insert_(key_type key, mapped_type value) override;

//     void free_(const_key_reference key) override;

//     const_mapped_reference at_(const_key_reference key) const override;

//     void backup_() override;

//     void dump_() override;

// private:
//     /// The object-to-UUID relationships we know about
//     map_pointer m_map_;

//     /// The DB we are backing-up our relationships to
//     backup_db_pointer m_backup_;
// };

// } // namespace pluginplay::database::detail_

// #include "uuid.ipp"
