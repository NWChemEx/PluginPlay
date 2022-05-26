#pragma once
#include "database_api.hpp"
#include <cstdlib>
#include <iostream>

namespace pluginplay::cache::database {

/** @brief The purpose of this class is to add a pre-defined key/value pair to
 *         a key.
 *
 *  This class assumes that @p KeyType is a type satisfying associative map. In
 *  this case `K = KeyType::key_type` and `V = KeyType::mapped_type` are then
 *  respectively the types of the keys and values in an instance of type
 *  @p KeyType. This class maintains a `std::pair<K, V>` instance which it
 *  automatically adds to every key that comes into the class.
 *
 *  In practice, this is used to inject the UUID of a module into the set of
 *  inputs so that we can track the module the inputs went into. However, the
 *  class is written in a relatively general manner (e.g., it could be made more
 *  general by storing an `std::map<K, V>` instance) as doing so adds little
 *  additional overhead and could prove useful down the line for adding
 *  additional metadata.
 *
 *  @tparam KeyType The types of the keys in this database. Assumed to satisfy
 *                  the concept of an associative map.
 *  @tparam ValueType The types of the values in this database.
 */
template<typename KeyType, typename ValueType>
class KeyInjector : public DatabaseAPI<KeyType, ValueType> {
private:
    /// Type of the database API this class satisfies
    using base_type = DatabaseAPI<KeyType, ValueType>;

    /// Type of the keys in KeyType
    using key_key_type = typename base_type::key_type::key_type;

    /// Type of the values in KeyType
    using key_value_type = typename base_type::key_type::mapped_type;

public:
    /// Type of this database's keys, typedef of KeyType
    using typename base_type::key_type;

    /// Read-only reference to a key, typedef of const KeyType&
    using typename base_type::const_key_reference;

    /// Type of this database's values
    using typename base_type::mapped_type;

    /// Type of an object holding a read-only reference to a value
    using typename base_type::const_mapped_reference;

    /// Type of the database we are wrapping
    using sub_db_type = base_type;

    /// Type of a pointer to the database we are wrapping
    using sub_db_pointer = std::shared_ptr<sub_db_type>;

    /** @brief Makes a new KeyInjector that will inject the provided key/value
     *         pair before adding keys to the provided database.
     *
     *  KeyInjectors wrap already existing database instances, but intercept
     *  keys before they go into the wrapped databse. This ctor takes the
     *  key/value pair to insert into the intercepted keys and the database to
     *  wrap.
     *
     *  @param[in] key_to_inject The key for the key/value pair that will be
     *                           injected into each key.
     *  @param[in] value_to_inject The value for the key/value pair that will be
     *                             injected into each key.
     *  @param[in] sub_db The database this database wraps. Expected to be an
     *                    already allocated databse.
     *
     *  @throw std::runtime_error if @p sub_db is null. Strong throw guarantee.
     *
     */
    KeyInjector(key_key_type key_to_inject, key_value_type value_to_inject,
                sub_db_pointer sub_db);

protected:
    /// injects into key, then calls m_db_->count()
    bool count_(const_key_reference key) const noexcept override;

    /// injects into key, then calls m_db_->insert
    void insert_(key_type key, mapped_type value) override;

    /// injects into key, then calls m_db_->free
    void free_(const_key_reference key) override;

    /// injects into key, then calls m_db_->at
    const_mapped_reference at_(const_key_reference key) const override;

    /// just calls m_db_->backup
    void backup_() override { m_db_->backup(); }

    /// just calls m_db_->dump
    void dump_() override { m_db_->dump(); }

private:
    /// Wraps the process of injecting the key/value pair into @p key
    key_type inject_(key_type key) const;

    /// The key of the key/value pair to inject
    key_key_type m_key_to_inject_;

    /// The value of the key/value pair to inject
    key_value_type m_value_to_inject_;

    /// The database we wrap
    sub_db_pointer m_db_;
};

} // namespace pluginplay::cache::database

#include "key_injector.ipp"
