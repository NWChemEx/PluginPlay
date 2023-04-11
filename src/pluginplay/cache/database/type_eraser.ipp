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

// This file meant only for inclusion from type_eraser.hpp

namespace pluginplay::cache::database {

#define TPARAMS template<typename KeyType, typename ValueType>
#define TYPE_ERASER TypeEraser<KeyType, ValueType>

TPARAMS
TYPE_ERASER::TypeEraser(wrapped_mapper_pointer db) : m_db_(db) {
    if(m_db_) return;
    throw std::runtime_error("Was expecting a non-null database.");
}

TPARAMS
typename TYPE_ERASER::key_set_type TYPE_ERASER::keys_() const {
    key_set_type rv;
    for(const auto& key : m_db_->keys())
        rv.push_back(any::any_cast<key_type>(key));
    return rv;
}

TPARAMS
bool TYPE_ERASER::count_(const_key_reference key) const noexcept {
    return m_db_->count(wrap_(key));
}

TPARAMS
void TYPE_ERASER::insert_(key_type key, mapped_type value) {
    m_db_->insert(wrap_(key), std::move(value));
}

TPARAMS
void TYPE_ERASER::free_(const_key_reference key) { m_db_->free(wrap_(key)); }

TPARAMS
typename TYPE_ERASER::const_mapped_reference TYPE_ERASER::at_(
  const_key_reference key) const {
    return m_db_->at(wrap_(key));
}

TPARAMS typename TYPE_ERASER::any_type TYPE_ERASER::wrap_(
  const_key_reference key) const {
    return MakeAny<key_type>::convert(key);
}

#undef TYPE_ERASER
#undef TPARAMS

} // namespace pluginplay::cache::database
