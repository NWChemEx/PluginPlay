/* RocksDB is designed to be an optional dependency. To accomplish this we need
 * to use preprocessor macros. Our goal is to localize them as much as possible.
 * To that end, we define two PIMPL implementations, rocksdb_pimpl.hpp contains
 * the real one which gets included when BUILD_ROCKS_DB is defined and
 * rocksdb_pimpl_stub.hpp gets included otherwise. The dispatch between the two
 * happens based on the PIMPL type in the RocksDB class.
 */

#include "detail_/rocksdb_pimpl_stub.hpp"
#ifdef BUILD_ROCKS_DB
#include "detail_/rocksdb_pimpl.hpp"
#endif

namespace pluginplay::cache::database {

#define TPARAMS template<typename KeyType, typename ValueType>
#define ROCKS_DB RocksDB<KeyType, ValueType>

TPARAMS
ROCKS_DB::RocksDB() noexcept = default;

TPARAMS
ROCKS_DB::RocksDB(const_path_reference path) :
  m_pimpl_(std::make_unique<pimpl_type>(path)) {}

TPARAMS
ROCKS_DB::~RocksDB() noexcept = default;

TPARAMS
bool ROCKS_DB::count_(const_key_reference key) const noexcept {
    if(!m_pimpl_) return false;
    return m_pimpl_->count(key);
}

TPARAMS
void ROCKS_DB::insert_(key_type key, mapped_type value) {
    pimpl_().insert(std::move(key), std::move(value));
}

TPARAMS
void ROCKS_DB::free_(const_key_reference key) { pimpl_().free(key); }

TPARAMS
typename ROCKS_DB::const_mapped_reference ROCKS_DB::at_(
  const_key_reference key) const {
    return pimpl_().at(key);
}

TPARAMS
void ROCKS_DB::backup_() {}

TPARAMS
void ROCKS_DB::dump_() {}

TPARAMS
void ROCKS_DB::assert_pimpl_() const {
    if(m_pimpl_) return;
    throw std::runtime_error("Object has no PIMPL. Was it default constructed "
                             "or moved from?");
}

TPARAMS
typename ROCKS_DB::pimpl_reference ROCKS_DB::pimpl_() {
    assert_pimpl_();
    return *m_pimpl_;
}

TPARAMS
typename ROCKS_DB::const_pimpl_reference ROCKS_DB::pimpl_() const {
    assert_pimpl_();
    return *m_pimpl_;
}

#undef ROCKS_DB
#undef TPARAMS

template class RocksDB<std::string, std::string>;

} // namespace pluginplay::cache::database
