#include "rocksdb_pimpl.hpp"

/* RocksDB is designed to be an optional dependency. To accomplish this we
 * hide all RocksDB-specific calls in this file behind the compiler definition
 * "BUILD_ROCKS_DB". If that definition is found the RocksDBPIMPL instance that
 * is defined will be fully functioning, if it's not any call to the RocksDB
 * PIMPL will raise an exception.
 */

#ifdef BUILD_ROCKS_DB
#include "rocksdb_pimpl.hpp"
#else
#include "rocksdb_pimpl_stub.hpp"
#endif

namespace pluginplay::database::detail_ {

#define TPARAMS template<typename KeyType, typename ValueType>
#define ROCKS_DB RocksDB<KeyType, ValueType>

TPARAMS
void ROCKS_DB::insert_(KeyType key, ValueType value) {
    assert_ptr_();
    if(value.size() > m_max_value_size_) {
        large_value_insert_(std::move(key), std::move(value));
        return;
    }
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
    if(m_split_values_.count(key)) { return large_value_at_(key); }

    auto opts = rocksdb::ReadOptions();
    ValueType buffer;
    auto status = m_db_->Get(opts, key, &buffer);
    assert(status.ok());
    return const_mapped_reference(std::move(buffer));
}

TPARAMS
void ROCKS_DB::assert_ptr_() const {
    if(m_db_) return;
    throw std::runtime_error("No allocated database. Was this PIMPL moved from"
                             " or default allocated?");
}

TPARAMS
auto ROCKS_DB::split_value_(ValueType& value) {
    // We just make iterator pairs in this function to avoid copies
    using itr_type  = typename ValueType::iterator;
    using pair_type = std::pair<itr_type, itr_type>;
    std::vector<pair_type> new_vals;
    std::size_t chunk_end = m_max_value_size_;
    auto begin_itr        = value.begin();
    while(chunk_end < value.size()) {
        auto end_itr = begin_itr + m_max_value_size_;
        new_vals.emplace_back(std::make_pair(begin_itr, end_itr));
        begin_itr += m_max_value_size_;
        chunk_end += m_max_value_size_;
    }

    // Still have one more write
    new_vals.emplace_back(std::make_pair(begin_itr, value.end()));
    return new_vals;
}

TPARAMS
void ROCKS_DB::large_value_insert_(KeyType key, ValueType value) {
    auto chunks = split_value_(value);

    // TODO: Use better sub keys (e.g. UUIDs)
    std::vector<KeyType> sub_keys(chunks.size(), key);

    for(std::size_t i = 0; i < chunks.size(); ++i) {
        auto& [begin, end] = chunks[i];
        sub_keys[i] += std::to_string(i);
        // TODO: Can we avoid the creation of a new ValueType?
        insert_(sub_keys[i], ValueType(begin, end));
    }
    m_split_values_.emplace(std::move(key), std::move(sub_keys));
}

TPARAMS
typename ROCKS_DB::const_mapped_reference ROCKS_DB::large_value_at_(
  const_key_reference key) const {
    ValueType buffer;
    for(const auto& sub_key : m_split_values_.at(key)) {
        auto sub_value = at_(sub_key);
        buffer += std::move(sub_value.get());
    }
    return const_mapped_reference(std::move(buffer));
}

#undef TPARAMS
#undef ROCKS_DB

template class RocksDB<std::string, std::string>;

} // namespace pluginplay::database::detail_
