#pragma once
#include "uuid_mapper.hpp"
#include <map>
namespace pluginplay::cache {

/** @brief This class takes one map-like type and maps it to another.
 *
 *  In our database hierarchy we often have keys/values which themselves are
 *  map-like. Let's say one of these maps has keys of type `K` and values of
 *  type `V`. This class's job is to go from the original map type to a new map
 *  type which still has keys of type `K`, but now has UUIDs as values. This
 *  is done via an intermediary UUIDMapper instance.
 *
 *  In practice we use this in two places. The first is to go from a map of
 *  module inputs to a map of proxies, the second is to go from a map of module
 *  result to a map of proxies.
 *
 *  @tparam KeyType The map we are mapping from. Assumed to be a specialization
 *                  of std::map
 *
 */
template<typename KeyType>
class ProxyMapMaker {
private:
    /// Type of the keys in the map we are mapping from
    using key_key_type = typename KeyType::key_type;

    /// Type of the values we are mapping from
    using key_value_type = typename KeyType::mapped_type;

    /// Type of the function used to compare keys
    using key_key_compare = typename KeyType::key_compare;

public:
    /// Type of the UUID Mapper
    using proxy_mapper = UUIDMapper<key_value_type>;

    /// Type of a uuid
    using uuid_type = typename proxy_mapper::mapped_type;

    /// Type of the keys, typedef of KeyType
    using key_type = KeyType;

    /// Ultimately a typedef of DatabaseAPI<key_value_type, ...>::key_set_type
    using key_set_type = std::vector<key_type>;

    /// Read-only reference to a key, typedef of const KeyType&
    using const_key_reference = const key_type&;

    /// Type of this database's values, typedef of ValueType
    using mapped_type = std::map<key_key_type, uuid_type, key_key_compare>;

    /// Type of a read-only reference to a proxy map
    using const_mapped_reference = const mapped_type&;

    /// Type of a pointer to a UUIDMapper
    using proxy_mapper_pointer = std::unique_ptr<proxy_mapper>;

    /** @brief Creates a new ProxyMapMaker which relies on @p db for making
     *         proxy objects.
     *
     *  This ctor creates a new ProxyMapMaker instance which wraps the provided
     *  UUIDMapper instance. When it comes time to map an object of type KeyType
     *  to an object of type ValueType, the created instance will use the
     *  mapping contained in @p db. N.B.  @p db is a shared_ptr so as to let
     *  other ProxyMapMaker instances agree on the UUIDs of values they've seen.
     *
     *  @param[in] db The UUIDMapper this instance will use for mapping.
     *
     *  @throw std::runtime_error if @p db is a null pointer. Strong throw
     *                            guarantee.
     */
    explicit ProxyMapMaker(proxy_mapper_pointer db);

    /** @brief Returns the set of objects which have been proxied.
     *
     *  This function returns the set of keys used as inputs, not the proxied
     *  versions (which are the values from the perspective of this class).
     *
     *  @return The set of objects which have been proxied.
     */
    key_set_type keys() const;

    /** @brief Determines if all the values in @p key are in the wrapped
     *         database.
     *
     *  ProxyMapMaker instances map values in the @p KeyType object to UUIDs.
     *  This function checks that we already have a UUID for each value in
     *  @p key. If we don't it returns false.
     *
     *  @param[in] key The incoming map whose values we're looking for.
     *
     *  @return True if every value in @p key has a UUID already assigned to it
     *          and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool count(const_key_reference key) const;

    /** @brief Assigns UUIDs to every value in @p key.
     *
     *  This function will loop over the key/value pairs in @p key and tell the
     *  wrapped UUIDMapper to generate a UUID for each value that currently
     *  does not have a UUID associated with it.
     *
     *  @param[in] key The map whose values will be added to the wrapped
     *             UUIDMapper instance.
     *
     *  @throw std::bad_alloc if there is a problem allocaitng memory for the
     *                        new key/value pair. Weak throw guarantee.
     */
    void insert(const_key_reference key);

    /** @brief Releases the value-to-UUID relationships for each value in @p key
     *
     *  Care needs to be taken when using this function as it will clear the
     *  UUIDs for each value in @p key. Since the wrapped UUIDMapper is in
     *  general used by other ProxyMapMaker instances those instances will also
     *  loose access to those UUIDs.
     *
     *  @param[in] key The map of key/values to release memory for. Each value
     *                 in @p key will have it's UUID released from the wrapped
     *                 database.
     *
     *  @throw ??? Throws if the backend throws. Same throw guarantee.
     */
    void free(const_key_reference key);

    /** @brief Returns a map of key-to-proxy objects.
     *
     *  This method will loop over the key/value pairs in @p key and generate a
     *  new map such that keys in the return are the same, but the values are
     *  the UUIDs the original values map to.
     *
     *  @param[in] key The map we are mapping to proxies.
     *
     *  @return A map where the values have been replaced by proxy objects.
     *
     *  @throw std::bad_alloc if there is problem making the return. Strong
     *                        throw guarantee.
     */
    mapped_type at(const_key_reference key) const;

    key_type un_proxy(const_mapped_reference value) const;

    /** @brief Saves the contents of the UUIDMapper.
     *
     *  Exactly what this function does depends on the database UUIDMapper uses
     *  for storing the results. This function will call backup on the
     *  UUIDMapper, which in turn will call backup on the database it wraps. In
     *  general that wrapped database will then save the results, while leaving
     *  the results in their native format.
     *
     *  @throw ??? If the backend database throws. Same throw guarantee.
     */
    void backup() { m_db_->backup(); }

    /** @brief Saves and releases the memory being used by the wrapped
     *         UUIDMapper.
     *
     *  This function simply calls dump on the wrapped UUIDMapper. Exactly what
     *  happens depends on the database UUIDMapper is wrapping. Generally
     *  speaking dump will free all memory of the UUIDMapper (and the wrapped
     *  database), after saving the contents.
     *
     *  @throw ??? Throws if the backend database throws. Same throw guarantee.
     */
    void dump() { m_db_->dump(); }

private:
    /// TODO: This is a hack so we can reverse the mapping
    std::map<mapped_type, key_type> m_buffer_;

    /// The instance preserving the UUID mapping
    proxy_mapper_pointer m_db_;
};

} // namespace pluginplay::cache

#include "proxy_map_maker.ipp"
