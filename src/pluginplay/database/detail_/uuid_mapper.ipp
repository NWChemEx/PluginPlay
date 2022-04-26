// This file is only meant for inclusion in uuid_mapper.hpp

namespace pluginplay::database::detail_ {

#define TPARAMS template<typename KeyType>
#define UUID_MAPPER UUIDMapper<KeyType>

TPARAMS
UUID_MAPPER::UUIDMapper(db_pointer db) : m_db_(std::move(db)) {
    if(!m_db_) throw std::runtime_error("Database can not be a nullptr");
}

TPARAMS
bool UUID_MAPPER::count_(const_key_reference key) const noexcept {
    return m_db_->count(key);
}

TPARAMS
void UUID_MAPPER::insert(key_type key) {
    if(count_(key)) return; // Don't regenerate the UUID
    insert_(std::move(key), uuid_());
}

TPARAMS
void UUID_MAPPER::insert_(key_type key, mapped_type value) {
    m_db_->insert(std::move(key), std::move(value));
}

TPARAMS
void UUID_MAPPER::free_(const_key_reference key) { m_db_->free(key); }

TPARAMS
typename UUID_MAPPER::const_mapped_reference UUID_MAPPER::at_(
  const_key_reference key) const {
    return m_db_->at(key);
}

TPARAMS
void UUID_MAPPER::backup_() {
    if(!m_db_) return; // No subdatabase
    m_db_->backup();
}

TPARAMS
void UUID_MAPPER::dump_() {
    if(!m_db_) return; // No subdatabase
    m_db_->dump();
}

TPARAMS
typename UUID_MAPPER::mapped_type UUID_MAPPER::uuid_() const {
    return boost::uuids::random_generator()();
}

#undef UUID_MAPPER
#undef TPARAMS

} // namespace pluginplay::database::detail_
