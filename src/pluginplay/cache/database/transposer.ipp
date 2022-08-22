// File meant only for inclusion in transposer.hpp

namespace pluginplay::cache::database {

#define TPARAMS template<typename KeyType, typename ValueType>
#define TRANSPOSER Transposer<KeyType, ValueType>

TPARAMS
TRANSPOSER::Transposer(wrapped_db_pointer p) : m_db_(std::move(p)) {
    if(!m_db_) throw std::runtime_error("Wrapped database can't be nullptr.");
}

TPARAMS
typename TRANSPOSER::key_set_type TRANSPOSER::keys_() const {
    key_set_type rv;
    for(const auto& val : m_keys_) rv.push_back(m_db_->at(val).get());
    return rv;
}

TPARAMS
bool TRANSPOSER::count_(const_key_reference key) const noexcept {
    for(const auto& val : m_keys_)
        if(m_db_->at(val).get() == key) return true;
    return false;
}

TPARAMS
void TRANSPOSER::insert_(key_type key, mapped_type value) {
    m_keys_.insert(value);
    m_db_->insert(std::move(value), std::move(key));
}

TPARAMS
void TRANSPOSER::free_(const_key_reference key) {
    for(const auto& val : m_keys_)
        if(m_db_->at(val).get() == key) {
            m_db_->free(val);
            m_keys_.erase(val);
            return;
        }
}

TPARAMS
typename TRANSPOSER::const_mapped_reference TRANSPOSER::at_(
  const_key_reference key) const {
    for(const auto& val : m_keys_)
        if(m_db_->at(val).get() == key) return const_mapped_reference{&val};
    throw std::out_of_range("Key not found");
}

TPARAMS
void TRANSPOSER::dump_() {
    m_db_->dump();
    m_keys_.clear();
}

#undef TRANSPOSER
#undef TPARAMS

} // namespace pluginplay::cache::database
