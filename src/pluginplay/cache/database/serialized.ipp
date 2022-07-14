// This file is meant only for inclusion from serialized.hpp

namespace pluginplay::cache::database {

#define TPARAMS template<typename KeyType, typename ValueType>
#define SERIALIZED Serialized<KeyType, ValueType>

TPARAMS
bool SERIALIZED::count_(const_key_reference key) const noexcept {
    try {
        return m_db_->count(serialize_(key));
    } catch(...) { return false; }
}

TPARAMS
void SERIALIZED::insert_(key_type key, mapped_type value) {
    auto skey = serialize_(std::move(key));
    auto sval = serialize_(std::move(value));
    m_db_->insert(std::move(skey), std::move(sval));
}

TPARAMS
void SERIALIZED::free_(const_key_reference key) {
    m_db_->free(serialize_(key));
}

TPARAMS
typename SERIALIZED::const_mapped_reference SERIALIZED::at_(
  const_key_reference key) const {
    auto serialized_key = serialize_(key);
    auto serialized_val = m_db_->at(serialized_key);
    auto rv             = deserialize_<mapped_type>(serialized_val.get());
    return const_mapped_reference(&rv);
}

TPARAMS
template<typename T>
typename SERIALIZED::binary_type SERIALIZED::serialize_(
  T&& serialize_me) const {
    std::stringstream ss;
    cereal::BinaryOutputArchive ar(ss);
    ar << std::forward<T>(serialize_me);
    return ss.str();
}

TPARAMS
template<typename T>
T SERIALIZED::deserialize_(const binary_type& deserialize_me) const {
    std::stringstream ss(deserialize_me);
    cereal::BinaryInputArchive ar(ss);
    T rv;
    ar >> rv;
    return rv;
}

#undef SERIALIZED
#undef TPARAMS

} // namespace pluginplay::cache::database
