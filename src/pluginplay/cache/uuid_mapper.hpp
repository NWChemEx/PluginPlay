#pragma once
#include "database/database_api.hpp"
#include <memory>
#include <pluginplay/utility/uuid.hpp>

namespace pluginplay::cache {

/** @brief Object responsible for assigning UUIDs to objects (and remembering
 *         them)
 *
 *  The UUIDMapper is responsible for assigning UUIDs to objects and maintaining
 *  a record of this mapping. Conceptually this means that UUIDMapper is
 *  viewable as a DatabasePIMPL<KeyType, boost::uuids::uuid> instance.
 *
 *  @tparam KeyType The type of the objects having UUIDs assigned to them.
 */
template<typename KeyType>
class UUIDMapper {
public:
    /// Type of the objects getting UUIDs assigned to them, typedef of KeyType
    using key_type = KeyType;

    /// Read-only reference to one of the objects, typedef of const KeyType&
    using const_key_reference = const key_type&;

    /// Type of the UUIDs, typedef of boost::uuids::uuid
    using mapped_type = std::string;

    /// Type of the database that UUIDMapper will store UUIDs in
    using db_type = database::DatabaseAPI<key_type, mapped_type>;

    /// Type of a read-only reference to a UUID
    using const_mapped_reference = typename db_type::const_mapped_reference;

    /// Type of a pointer to the database storing the object-to-UUID relation
    using db_pointer = std::unique_ptr<db_type>;

    /// Type of a container holding keys
    using key_set_type = typename db_type::key_set_type;

    /** @brief Creates a new UUID instance which stores the UUID mapping in the
     *         provided db
     *
     *  @param[in] db The database that UUID will store object-to-UUID mappings
     *                in.
     *
     *  @throw std::runtime_error if @p db is a nullptr. Strong throw guarantee.
     */
    UUIDMapper(db_pointer db);

    /** @brief Overloads insert so that the user doesn't need to provide a UUID.
     *
     *  The insert method of the base class takes both a key and a value. That
     *  method is useful for reloading a saved state into a UUIDMapper instance.
     *  For initially generating the state this overload is more useful. This
     *  overload calls the internal uuid_ function (which in turn creates a new
     *  UUID) and forwards the key, and the newly created UUID, to the base
     *  class's insert method.
     *
     *  N.B. To help avoid accidentally overwriting a UUID this function is a
     *       no-op if `count(key)` is true. If you really want to overwrite the
     *       UUID call `free(key)` first.
     *
     *  @param[in] key The object getting a UUID assigned to it. If @p key
     *                 already has a UUID this is a no-op.
     *
     *  @throw boost::uuids::entropy_error if there is a problem generating the
     *         UUID due to insufficient entropy. Strong throw guarantee.
     *
     *  @throw ??? If the wrapped database's insert method throws. Same throw
     *         gurantee.
     */
    void insert(key_type key);

    /** @brief Returns the set of objects which have been proxied.
     *
     *  N.B. this operation should only be used for debugging as it will copy
     *  each key into the returned object.
     *
     *  @return A container with copies of each key.
     *
     *  @throw std::bad_alloc if there is a problem allocating the return.
     *         Strong throw guarantee.
     */
    key_set_type keys() const { return m_db_->keys(); }

    /// Just calls m_db_->count(key)
    bool count(const_key_reference key) const noexcept;

    /// Just calls m_db_->free(key)
    void free(const_key_reference key);

    /// Just calls m_db_->at(key)
    const_mapped_reference at(const_key_reference key) const;

    /// Just calls m_db_->backup()
    void backup();

    /// Just calls m_db_->dump()
    void dump();

private:
    /** @brief Wraps the process of generating a UUID
     *
     *  At the moment this just wraps a call to Boost's random UUID generator.
     *
     *  @throw boost::uuids::entropy_error if the backend can't generate enough
     *         entropy. Strong throw guarantee.
     */
    mapped_type uuid_() const;

    /// The object-to-UUID relationships we know about
    db_pointer m_db_;
};

} // namespace pluginplay::cache

#include "uuid_mapper.ipp"
