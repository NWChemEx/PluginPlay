// This file meant only for inclusion from key_proxy_mapper.hpp

namespace pluginplay::cache ::database {
#define TPARAMS template<typename KeyType, typename ValueType>
#define KEY_PROXY_MAPPER KeyProxyMapper<KeyType, ValueType>

TPARAMS
KEY_PROXY_MAPPER::KeyProxyMapper(proxy_map_maker_pointer proxy_mapper,
                                 sub_db_pointer sub_db) :
  m_proxy_mapper_(std::move(proxy_mapper)), m_sub_db_(std::move(sub_db)) {
    if(m_proxy_mapper_ && m_sub_db_) return;
    throw std::runtime_error("Expected non-null databases");
}

TPARAMS
bool KEY_PROXY_MAPPER::count_(const_key_reference key) const noexcept {
    // Each part of key needs to be in proxy_mapper or it can't be in sub_db
    if(!m_proxy_mapper_->count(key)) return false;
    // TODO: I think the at call can throw
    return m_sub_db_->count(m_proxy_mapper_->at(key));
}

TPARAMS
void KEY_PROXY_MAPPER::insert_(key_type key, mapped_type value) {
    m_proxy_mapper_->insert(key);
    m_sub_db_->insert(m_proxy_mapper_->at(key), std::move(value));
}

TPARAMS
void KEY_PROXY_MAPPER::free_(const_key_reference key) {
    m_sub_db_->free(m_proxy_mapper_->at(key));
}

TPARAMS
typename KEY_PROXY_MAPPER::const_mapped_reference KEY_PROXY_MAPPER::at_(
  const_key_reference key) const {
    return m_sub_db_->at(m_proxy_mapper_->at(key));
}

TPARAMS
void KEY_PROXY_MAPPER::backup_() {
    m_proxy_mapper_->backup();
    m_sub_db_->backup();
}

TPARAMS
void KEY_PROXY_MAPPER::dump_() {
    // TODO: not sure if proxy_mapper should dump too. N.B. free doesn't release
    // so current behavior is consistent with that
    m_proxy_mapper_->backup();
    m_sub_db_->dump();
}

#undef KEY_PROXY_MAPPER
#undef TPARAMS

} // namespace pluginplay::cache::database
