// File to be included from native.hpp only

namespace pluginplay::cache::database {

#define TPARAMS template<typename KeyType, typename ValueType>
#define NATIVE Native<KeyType, ValueType>

TPARAMS
NATIVE::Native(map_type map, backup_db_pointer backup) :
  m_map_(std::move(map)), m_backup_(std::move(backup)) {}

TPARAMS
NATIVE::Native(backup_db_pointer backup) :
  Native(map_type{}, std::move(backup)) {}

TPARAMS
bool NATIVE::count_(const_key_reference key) const noexcept {
    return m_map_.count(key);
}

TPARAMS
void NATIVE::insert_(key_type key, mapped_type value) {
    m_map_[std::move(key)] = std::move(value);
}

TPARAMS
void NATIVE::free_(const_key_reference key) { m_map_.erase(key); }

TPARAMS
typename NATIVE::const_mapped_reference NATIVE::at_(
  const_key_reference key) const {
    return const_mapped_reference(&m_map_.at(key));
}

TPARAMS
void NATIVE::backup_() {
    if(!m_backup_) return;
    for(const auto& [k, v] : m_map_) m_backup_->insert(k, v);
}

TPARAMS
void NATIVE::dump_() {
    backup_();
    m_map_.clear();
}

#undef NATIVE
#undef TPARAMS

} // namespace pluginplay::cache::database
