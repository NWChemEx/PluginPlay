#pragma once
#include "../../../config/config_impl.hpp" // For with_rockdb_v
#include "../database_api.hpp"
#include <memory>
#include <string>

namespace pluginplay::cache::database {
namespace detail_ {
class RocksDBPIMPL;
class RocksDBPIMPLStub;
} // namespace detail_

/** @brief Wraps RocksDB in PluginPlay's database API.
 *
 *  This DatabaseAPI can be used to implement a Database whose data is managed
 *  by RocksDB. The RocksDB class has its own PIMPL which wraps the calls to
 *  RocksDB, and handles dispatch if the user opted not to build PluginPlay with
 *  RocksDB support. In the case that PluginPlay was built without RocksDB
 *  support attempting to use this class will result in runtime errors.
 *
 *  N.B. API documentation assumes that RocksDB support has been enabled. If it
 *  has not, then attempting to call any method other than the default ctor or
 *  the dtor will result in a runtime error.
 *
 *  @tparam KeyType Type of the keys in the database, expected to be some type
 *                  which holds binary data.
 *  @tparam ValueType Type of the values that the keys map to. Exected to be a
 *                    type holding binary data.
 */
template<typename KeyType, typename ValueType>
class RocksDB : public DatabaseAPI<KeyType, ValueType> {
private:
    /// Type this class implements
    using base_type = DatabaseAPI<KeyType, ValueType>;

public:
    /// Type used for specifying the disk location of the database
    using path_type = std::string;

    /// Type of a read-only reference to the disk location
    using const_path_reference = const path_type&;

    /// @copydoc base_type::key_type
    using key_type = typename base_type::key_type;

    /// @copydoc base_type::const_key_reference
    using const_key_reference = typename base_type::const_key_reference;

    /// @copydoc base_type::mapped_type
    using mapped_type = typename base_type::mapped_type;

    /// @copydoc base_type::const_mapped_reference
    using const_mapped_reference = typename base_type::const_mapped_reference;

    /** @brief Creates a stub RocksDB instance.
     *
     *  The instance resulting from this ctor has no PIMPL and can not be used
     *  except as a place holder.
     *
     *  @throw None No throw guarantee.
     */
    RocksDB() noexcept;

    /** @brief Creates a, or opens an existing, RocksDB database
     *
     *  This Ctor is used to open an existing database (if @p path
     *  already exists and is a RocksDB database) or to create a new database
     * (if
     *  @p path does not point to an existing file). Errors will arise if the
     *  RockSDB backend can not open the database (@p path already exists), or
     *  if the backend can not create the database. If this call is sucessful
     *  then the database is ready for business.
     *
     *  @param[in] path Where the RocksDB database lives/will live. If @p path
     *                  is an already existing RocksDB database the resulting
     *                  instance will open it. If @p path is not an existing
     *                  database then a new database will be created and opend.
     *
     *  @throw std::bad_alloc if the PIMPL can not be created. Strong throw
     *                        guarantee.
     */
    explicit RocksDB(const_path_reference path);

    /** @brief Default Dtor
     *
     *  @throw None no throw guarantee.
     */
    ~RocksDB() noexcept;

protected:
    /// Implements count method
    bool count_(const_key_reference key) const noexcept override;

    /// Implements insert method
    void insert_(key_type key, mapped_type value) override;

    /// Implements free method
    void free_(const_key_reference key) override;

    /// Implements at and operator[]
    const_mapped_reference at_(const_key_reference key) const override;

    /// Implements backup (which ATM is a no-op)
    void backup_() override;

    /// Implements dump (which ATM is a no-op)
    void dump_() override;

private:
    /// Type of the implementation
    using pimpl_type = std::conditional_t<with_rocksdb_v, detail_::RocksDBPIMPL,
                                          detail_::RocksDBPIMPLStub>;

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

} // namespace pluginplay::cache::database
