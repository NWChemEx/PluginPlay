// This file is only meant for inclusion in uuid_mapper.hpp

namespace pluginplay::cache {

#define TPARAMS template<typename KeyType>
#define UUID_MAPPER UUIDMapper<KeyType>

TPARAMS
UUID_MAPPER::UUIDMapper(db_pointer db) : m_db_(std::move(db)) {
    if(!m_db_) throw std::runtime_error("Database can not be a nullptr");
}

TPARAMS
bool UUID_MAPPER::count(const_key_reference key) const noexcept {
    return m_db_->count(key);
}

TPARAMS
void UUID_MAPPER::insert(key_type key) {
    if(count(key)) return; // Don't regenerate the UUID
    m_db_->insert(std::move(key), uuid_());
}

TPARAMS
void UUID_MAPPER::free(const_key_reference key) { m_db_->free(key); }

TPARAMS
typename UUID_MAPPER::const_mapped_reference UUID_MAPPER::at(
  const_key_reference key) const {
    return m_db_->at(key);
}

TPARAMS
void UUID_MAPPER::backup() { m_db_->backup(); }

TPARAMS
void UUID_MAPPER::dump() { m_db_->dump(); }

TPARAMS
typename UUID_MAPPER::mapped_type UUID_MAPPER::uuid_() const {
    return utility::generate_uuid();
}

#undef UUID_MAPPER
#undef TPARAMS

} // namespace pluginplay::cache
