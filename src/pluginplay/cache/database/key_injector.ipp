// This file is meant only for inclusion in key_injector.hpp

namespace pluginplay::cache::database {

#define TPARAMS template<typename KeyType, typename ValueType>
#define KEY_INJECTOR KeyInjector<KeyType, ValueType>

TPARAMS
KEY_INJECTOR::KeyInjector(key_key_type key_to_inject,
                          key_value_type value_to_inject,
                          sub_db_pointer sub_db) :
  m_key_to_inject_(std::move(key_to_inject)),
  m_value_to_inject_(std::move(value_to_inject)),
  m_db_(sub_db) {
    if(sub_db) return;
    throw std::runtime_error("Expected a non-null database to wrap.");
}

TPARAMS
bool KEY_INJECTOR::count_(const_key_reference key) const noexcept {
    // This actually can throw if there's not enough memory
    try {
        return m_db_->count(inject_(key));
    } catch(...) {
        std::cerr << "An uncaught exception was raised in KeyInjector::count_";
        std::abort();
    }
}

TPARAMS
void KEY_INJECTOR::insert_(key_type key, mapped_type value) {
    m_db_->insert(inject_(std::move(key)), std::move(value));
}

TPARAMS
void KEY_INJECTOR::free_(const_key_reference key) { m_db_->free(inject_(key)); }

TPARAMS
typename KEY_INJECTOR::const_mapped_reference KEY_INJECTOR::at_(
  const_key_reference key) const {
    return m_db_->at(inject_(key));
}

TPARAMS
typename KEY_INJECTOR::key_type KEY_INJECTOR::inject_(key_type key) const {
    key.emplace(m_key_to_inject_, m_value_to_inject_);
    return std::move(key);
}

#undef KEY_INJECTOR
#undef TPARAMS

} // namespace pluginplay::cache::database
