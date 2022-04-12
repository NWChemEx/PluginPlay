#pragma once
#include "database_pimpl.hpp"
#include <rocksdb/db.h>

namespace pluginplay::database::detail_ {

/** @brief Wraps RocksDB in PluginPlay's database API.
 *
 */
template<typename KeyType, typename ValueType>
class RocksDB : public DatabasePIMPL<KeyType, ValueType> {
public:
    using db_type    = rocksdb::DB;
    using db_pointer = db_type*;

    using options_type = rocksdb::Options;

    using path_type = std::string;

    using const_path_reference = const path_type&;

    using base_type = DatabasePIMPL<KeyType, ValueType>;

    using const_key_reference    = typename base_type::const_key_reference;
    using const_mapped_reference = typename base_type::const_mapped_reference;

    RocksDB() = default;

    explicit RocksDB(const_path_reference path);

    static auto options();
    static auto allocate(const_path_reference path, options_type opts);

protected:
    bool count_(const_key_reference key) const noexcept override;

    void insert_(KeyType key, ValueType value) override;

    void free_(const_key_reference key) override;

    const_mapped_reference at_(const_key_reference key) const override;

    void backup_() override {}

    void dump_() override {}

private:
    void assert_ptr_() const;

    struct Deleter {
        void operator()(db_pointer db) const;
    };

    std::unique_ptr<db_type, Deleter> m_db_;
};

#define TPARAMS template<typename KeyType, typename ValueType>
#define ROCKS_DB RocksDB<KeyType, ValueType>

TPARAMS
ROCKS_DB::RocksDB(const_path_reference path) :
  m_db_(allocate(path, options())) {}

TPARAMS
auto ROCKS_DB::options() {
    options_type options;
    options.create_if_missing = true; // Make DB if it DNE
    return options;
}

TPARAMS
auto ROCKS_DB::allocate(const_path_reference path, options_type opts) {
    db_pointer db;
    auto status = rocksdb::DB::Open(std::move(opts), path, &db);
    assert(status.ok());
    return db;
}

TPARAMS
bool ROCKS_DB::count_(const_key_reference key) const noexcept {
    assert_ptr_();
    auto opts = rocksdb::ReadOptions();

    // Rule out that it definitely doesn't exist
    if(!m_db->KeyMayExist(opts, key, nullptr)) return false;

    ValueType buffer;
    auto status = m_db_->Get(opts, key, &buffer);
    return ValueType{} != buffer;
}

TPARAMS
void ROCKS_DB::insert_(KeyType key, ValueType value) {
    assert_ptr_();
    auto opts   = rocksdb::WriteOptions();
    auto status = m_db_->Put(std::move(opts), std::move(key), std::move(value));
    assert(status.ok());
}

TPARAMS
void ROCKS_DB::free_(const_key_reference key) {
    assert_ptr_();
    auto opts   = rocksdb::WriteOptions();
    auto status = m_db_->Delete(opts, std::move(key));
    assert(status.ok());
}

TPARAMS
typename ROCKS_DB::const_mapped_reference ROCKS_DB::at_(
  const_key_reference key) const {
    assert_ptr_();
    auto opts = rocksdb::ReadOptions();
    ValueType buffer;
    auto status = m_db_->Get(opts, key, &buffer);
    assert(status.ok());
    return const_mapped_reference(std::move(buffer));
}

TPARAMS
void ROCKS_DB::Deleter::operator()(db_pointer db) const {
    db->Close();
    delete db;
}

TPARAMS
void ROCKS_DB::assert_ptr_() const {
    if(m_db_) return;
    throw std::runtime_error("No allocated database. Was this PIMPL moved from"
                             " or default allocated?");
}

#undef TPARAMS
#undef ROCKS_DB

} // namespace pluginplay::database::detail_
