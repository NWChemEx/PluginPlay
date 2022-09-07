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

// To be included only from proxy_map_maker.hpp

namespace pluginplay::cache {

#define TPARAMS template<typename KeyType>
#define PROXY_MAP_MAKER ProxyMapMaker<KeyType>

TPARAMS
PROXY_MAP_MAKER::ProxyMapMaker(proxy_mapper_pointer db) : m_db_(std::move(db)) {
    if(m_db_) return;
    throw std::runtime_error("Expected a non-null DB to use");
}

TPARAMS
typename PROXY_MAP_MAKER::key_set_type PROXY_MAP_MAKER::keys() const {
    key_set_type rv;
    for(const auto& [_, v] : m_buffer_) rv.push_back(v);
    return rv;
}

TPARAMS bool PROXY_MAP_MAKER::count(const_key_reference key) const {
    for(const auto& [_, v] : key) {
        if(!m_db_->count(v)) return false;
    }
    return true;
}

TPARAMS
void PROXY_MAP_MAKER::insert(const_key_reference key) {
    for(const auto& [k, v] : key) { m_db_->insert(v); }
    m_buffer_.emplace(at(key), key);
}

TPARAMS
void PROXY_MAP_MAKER::free(const_key_reference key) {
    for(const auto& [_, v] : key) { m_db_->free(v); }
}

TPARAMS
typename PROXY_MAP_MAKER::mapped_type PROXY_MAP_MAKER::at(
  const_key_reference key) const {
    mapped_type rv;
    for(const auto& [k, v] : key) { rv.emplace(k, m_db_->at(v).get()); }
    return rv;
}

TPARAMS
typename PROXY_MAP_MAKER::key_type PROXY_MAP_MAKER::un_proxy(
  const_mapped_reference value) const {
    return m_buffer_.at(value);
}

#undef PROXY_MAP_MAKER
#undef TPARAMS

} // namespace pluginplay::cache
