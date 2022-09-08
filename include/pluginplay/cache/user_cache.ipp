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

// This file meant only for inclusion from user_cache.hpp

namespace pluginplay::cache {

template<typename T>
bool UserCache::count(T&& key) const {
    auto wrap_key = wrap_inputs_(std::forward<T>(key));
    return m_cache_.count(wrap_key);
}

template<typename T, typename U>
void UserCache::cache(T&& key, U&& value) {
    auto wrap_key = wrap_inputs_(std::forward<T>(key));
    auto wrap_val = wrap_results_(std::forward<U>(value));
    m_cache_.cache(std::move(wrap_key), std::move(wrap_val));
}

template<typename U, typename T>
U UserCache::uncache(T&& key) {
    auto wrap_key = wrap_inputs_(std::forward<T>(key));
    auto wrap_val = m_cache_.uncache(std::move(wrap_key));
    return unwrap_results_<U>(std::move(wrap_val));
}

template<typename U, typename T, typename V>
U UserCache::uncache(T&& key, V&& default_value) {
    return count(key) ? uncache<U>(std::forward<T>(key)) : default_value;
}

template<typename T>
typename UserCache::input_map_type UserCache::wrap_inputs_(T&& key) const {
    using input_type = input_map_type::mapped_type;
    input_type i;
    i.set_type<std::decay_t<T>>();
    i.change(std::forward<T>(key));
    return input_map_type{{"", std::move(i)}};
}

template<typename T>
typename UserCache::result_map_type UserCache::wrap_results_(T&& value) const {
    using result_type = result_map_type::mapped_type;
    result_type r;
    r.set_type<std::decay_t<T>>();
    r.change(std::forward<T>(value));
    return result_map_type{{"", std::move(r)}};
}

template<typename T>
T UserCache::unwrap_results_(result_map_type value) const {
    return value.at("").value<T>();
}

} // namespace pluginplay::cache
