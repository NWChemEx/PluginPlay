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
typename NATIVE::key_set_type NATIVE::keys_() const {
    key_set_type rv;
    for(const auto& [k, _] : m_map_) rv.push_back(k);
    return rv;
}

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
