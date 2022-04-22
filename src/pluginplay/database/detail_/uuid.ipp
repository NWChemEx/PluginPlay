// // This file is only meant for inclusion in uuid.hpp

// namespace pluginplay::database::detail_ {

// #define TPARAMS template<typename KeyType>
// #define UUID_ UUID<KeyType>

// TPARAMS
// UUID_::UUID(map_pointer map, backup_db_pointer backup) noexcept :
//   m_map_(std::move(map)), m_backup_(std::move(backup)) {}

// TPARAMS
// bool UUID_::count_(const_key_reference key) const noexcept {
//     return m_map_->count(key);
// }

// TPARAMS
// void UUID_::insert_(key_type key, mapped_type value) {
//     m_map_->insert(std::move(key), std::move(value));
// }

// TPARAMS
// void UUID_::free_(const_key_reference key) { m_map_->free(key); }

// TPARAMS
// typename UUID_::const_mapped_reference UUID_::at_(
//   const_key_reference key) const {
//     return m_map_->at(key);
// }

// TPARAMS
// void UUID_::backup_() {
//     if(!m_backup_) return; // No subdatabase
// }

// TPARAMS
// void UUID_::dump_() {
//     if(!m_backup_) return; // No subdatabase
// }

// #undef UUID_
// #undef TPARAMS

// } // namespace pluginplay::database::detail_
