/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This file is meant only for inclusion from serialized.hpp

namespace pluginplay::cache::database {

#define TPARAMS template<typename KeyType, typename ValueType>
#define SERIALIZED Serialized<KeyType, ValueType>

TPARAMS
typename SERIALIZED::key_set_type SERIALIZED::keys_() const {
    key_set_type rv;
    for(const auto& k : m_db_->keys()) rv.push_back(deserialize_<key_type>(k));
    return rv;
}

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
    return const_mapped_reference(std::move(rv));
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
