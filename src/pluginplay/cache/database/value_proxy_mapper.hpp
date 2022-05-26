#pragma once
#include "../proxy_map_maker.hpp"
#include "database_api.hpp"

namespace pluginplay::cache::database {

/** @brief Automatically proxies the values before storing them.
 *
 *  This class uses the wrapped ProxyMapMaker instance to map the incoming v
 *  alues to proxy maps before storing them in the wrapped database.
 *
 *  @tparam KeyType The type of the keys users provide this database. They just
 *                  pass through.
 *  @tparam VlueType The type of values users provide this database. They are
 *                   expected to be specializations of std::map.
 *
 */
template<typename KeyType, typename ValueType>
class ValueProxyMapper : public DatabaseAPI<KeyType, ValueType> {
private:
    /// Type of the API this class implements
    using base_type = DatabaseAPI<KeyType, ValueType>;

public:
    /// Type of the keys, typedef of KeyType
    using typename base_type::key_type;

    /// Read-only reference to a key, typedef of const KeyType&
    using typename base_type::const_key_reference;

    /// Type of the values, typedef of ValueType
    using typename base_type::mapped_type;

    /// Type of an object holding a read-only reference to a value
    using typename base_type::const_mapped_reference;

    /// Type the ProxyMapMaker used for assigning proxies
    using proxy_map_maker = ProxyMapMaker<mapped_type>;

    /// Type that the ProxyMapMaker maps to
    using proxy_map_type = typename proxy_map_maker::mapped_type;

    /// Type of a pointer to the ProxyMapMaker
    using proxy_map_maker_pointer = std::unique_ptr<proxy_map_maker>;

    /// Type of the database we wrap
    using sub_db_type = DatabaseAPI<key_type, proxy_map_type>;

    /// Type of a pointer to the database we wrap
    using sub_db_pointer = std::unique_ptr<sub_db_type>;

    /** @brief Creates a new ValueProxyMapper with the provided state.
     *
     *  @param[in] proxy_mapper A non-null pointer to the ProxyMapMaker instance
     *                          that the resulting instance will use for mapping
     *                          values to proxies.
     *  @param[in] sub_db A non-null pointer to a database satisfying the API
     *                    `sub_db_type`. This is the database where the proxy
     *                    map to value relationships will be stored.
     */
    ValueProxyMapper(proxy_map_maker_pointer proxy_mapper,
                     sub_db_pointer sub_db);

protected:
    /// Makes sure key is in proxy_mapper, if so then check sub_db
    bool count_(const_key_reference key) const noexcept override;

    /// Inserts into proxy_mapper, then into sub_db
    void insert_(key_type key, mapped_type value) override;

    /// Removes key from sub_db only
    void free_(const_key_reference key) override;

    /// Uses proxy_mapper to map key, before calling sub_db
    const_mapped_reference at_(const_key_reference key) const override;

    /// Just calls backup on both proxy_mapper and sub_db
    void backup_() override;

    /// backs proxy_mapper up and dumps sub_db
    void dump_() override;

private:
    /// Used to map keys to proxy maps
    proxy_map_maker_pointer m_proxy_mapper_;

    /// Stores proxy map to value relationships
    sub_db_pointer m_sub_db_;
};

} // namespace pluginplay::cache::database

#include "value_proxy_mapper.ipp"
