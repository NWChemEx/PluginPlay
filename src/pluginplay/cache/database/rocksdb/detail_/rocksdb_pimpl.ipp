// This file is meant only for inclusion from rocksdb_pimpl.hpp
namespace pluginplay::cache::database::detail_ {

// Macro which hides inline, but could be used to hid template parameters
#define TPARAMS inline

// Macro which hides the class name (including template parameters if added)
#define ROCKSDB_PIMPL RocksDBPIMPL

TPARAMS
ROCKSDB_PIMPL::ROCKSDB_PIMPL(const_path_reference path) :
  m_db_(allocate_(path, options_())) {}

TPARAMS
bool ROCKSDB_PIMPL::count(const_key_reference key) const noexcept {
    assert_ptr_();
    if(m_split_values_.count(key)) return true;
    auto opts = rocksdb::ReadOptions();

    mapped_type buffer;

    // Rule out that it definitely doesn't exist
    if(!m_db_->KeyMayExist(opts, key, &buffer)) return false;

    auto status = m_db_->Get(opts, key, &buffer);
    return mapped_type{} != buffer;
}

TPARAMS
void ROCKSDB_PIMPL::insert(key_type key, mapped_type value) {
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
void ROCKSDB_PIMPL::free(const_key_reference key) {
    assert_ptr_();
    if(m_split_values_.count(key)) {
        large_value_free_(key);
        return;
    }
    auto opts   = rocksdb::WriteOptions();
    auto status = m_db_->Delete(opts, std::move(key));
    assert(status.ok());
}

TPARAMS
typename ROCKSDB_PIMPL::const_mapped_reference ROCKSDB_PIMPL::at(
  const_key_reference key) const {
    assert_ptr_();
    if(m_split_values_.count(key)) { return large_value_at_(key); }

    auto opts = rocksdb::ReadOptions();
    mapped_type buffer;
    auto status = m_db_->Get(opts, key, &buffer);

    if(!status.ok()) throw std::out_of_range(status.ToString());
    return const_mapped_reference(std::move(buffer));
}

TPARAMS
typename ROCKSDB_PIMPL::options_type ROCKSDB_PIMPL::options_() {
    options_type options;
    options.create_if_missing = true; // Make DB if it DNE
    return options;
}

TPARAMS
typename ROCKSDB_PIMPL::raw_db_pointer ROCKSDB_PIMPL::allocate_(
  const_path_reference path, options_type opts) {
    raw_db_pointer db;
    auto status = rocksdb::DB::Open(std::move(opts), path, &db);
    assert(status.ok());
    return db;
}

TPARAMS
void ROCKSDB_PIMPL::assert_ptr_() const {
    if(m_db_) return;
    throw std::runtime_error("No allocated database. Was this PIMPL moved from"
                             " or default allocated?");
}

TPARAMS
auto ROCKSDB_PIMPL::split_value_(mapped_reference value) {
    // We just make iterator pairs in this function to avoid copies
    using itr_type  = typename mapped_type::iterator;
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
void ROCKSDB_PIMPL::large_value_insert_(key_type key, mapped_type value) {
    auto chunks = split_value_(value);

    // TODO: Use better sub keys (e.g. UUIDs)
    std::vector<key_type> sub_keys(chunks.size(), key);

    for(std::size_t i = 0; i < chunks.size(); ++i) {
        auto& [begin, end] = chunks[i];
        sub_keys[i] += std::to_string(i);
        // TODO: Can we avoid the creation of a new mapped_type?
        insert(sub_keys[i], mapped_type(begin, end));
    }
    m_split_values_.emplace(std::move(key), std::move(sub_keys));
}

TPARAMS
typename ROCKSDB_PIMPL::const_mapped_reference ROCKSDB_PIMPL::large_value_at_(
  const_key_reference key) const {
    mapped_type buffer;
    for(const auto& sub_key : m_split_values_.at(key)) {
        auto sub_value = at(sub_key);
        buffer += std::move(sub_value.get());
    }
    return const_mapped_reference(std::move(buffer));
}

TPARAMS
void ROCKSDB_PIMPL::large_value_free_(const_key_reference key) {
    for(const auto& sub_key : m_split_values_.at(key)) free(sub_key);
    m_split_values_.erase(key);
}

#undef ROCKSDB_PIMPL
#undef TPARAMS

} // namespace pluginplay::cache::database::detail_
