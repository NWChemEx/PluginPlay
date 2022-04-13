#pragma once
#include "database_pimpl.hpp"
#include <string>

namespace pluginplay::database::detail_ {
struct RocksDBPIMPL;

/** @brief Wraps RocksDB in PluginPlay's database API.
 *
 */
template<typename KeyType, typename ValueType>
class RocksDB : public DatabasePIMPL<KeyType, ValueType> {
public:
    using path_type = std::string;

    using const_path_reference = const path_type&;

    using base_type = DatabasePIMPL<KeyType, ValueType>;

    using const_key_reference    = typename base_type::const_key_reference;
    using const_mapped_reference = typename base_type::const_mapped_reference;

    RocksDB() = default;

    explicit RocksDB(const_path_reference path);

protected:
    bool count_(const_key_reference key) const noexcept override;

    void insert_(KeyType key, ValueType value) override;

    void free_(const_key_reference key) override;

    const_mapped_reference at_(const_key_reference key) const override;

    void backup_() override {}

    void dump_() override {}

private:
    std::unique_ptr<RocksDBPIMPL> m_pimpl_;
};

extern template class RocksDB<std::string, std::string>;

} // namespace pluginplay::database::detail_
