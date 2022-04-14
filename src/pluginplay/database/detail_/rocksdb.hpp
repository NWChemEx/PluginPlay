#pragma once
#include "database_pimpl.hpp"
#include <memory>
#include <string>

namespace pluginplay::database::detail_ {
class RocksDBPIMPL;

/** @brief Wraps RocksDB in PluginPlay's database API.
 *
 *  This DatabasePIMPL can be used to implement a Database whose data is managed
 *  by RocksDB. The RocksDB class has its own PIMPL which wraps the calls to
 *  RocksDB, and handles dispatch if the user opted not to build PluginPlay with
 *  RocksDB support. In the case that PluginPlay was built without RocksDB
 *  support attempting to use this class will result in runtime errors.
 *
 *  @tparam
 */
template<typename KeyType, typename ValueType>
class RocksDB : public DatabasePIMPL<KeyType, ValueType> {
private:
    /// Type this class implements
    using base_type = DatabasePIMPL<KeyType, ValueType>;

public:
    /// Type used for specifying the disk location of the database
    using path_type = std::string;

    /// Type of a read-only reference to the disk location
    using const_path_reference = const path_type&;

    using key_type = typename base_type::key_type;

    using const_key_reference = typename base_type::const_key_reference;

    using mapped_type = typename base_type::mapped_type;

    using const_mapped_reference = typename base_type::const_mapped_reference;

    RocksDB() noexcept;

    ~RocksDB() noexcept;

    explicit RocksDB(const_path_reference path);

protected:
    bool count_(const_key_reference key) const noexcept override;

    void insert_(key_type key, mapped_type value) override;

    void free_(const_key_reference key) override;

    const_mapped_reference at_(const_key_reference key) const override;

    void backup_() override;

    void dump_() override;

private:
    /// Type of the implementation
    using pimpl_type = RocksDBPIMPL;

    /// Type of a mutable reference to the PIMPL
    using pimpl_reference = pimpl_type&;

    /// Type of a read-only reference to the PIMPL
    using const_pimpl_reference = const pimpl_type&;

    /// Type of a mutable pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Factors out throwing if a PIMPL has not been allocated
    void assert_pimpl_() const;

    /// Calls assert_pimpl_ then returns a mutable PIMPL
    pimpl_reference pimpl_();

    /// Calls assert_pimpl_ then returns an immutable PIMPL
    const_pimpl_reference pimpl_() const;

    /// The actual implementation
    pimpl_pointer m_pimpl_;
};

extern template class RocksDB<std::string, std::string>;

} // namespace pluginplay::database::detail_
