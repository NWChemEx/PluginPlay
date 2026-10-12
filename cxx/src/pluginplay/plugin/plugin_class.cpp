/*
 * Copyright 2026 NWChemEx-Project
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

#include <algorithm>
#include <pluginplay/plugin/plugin_class.hpp>
#include <set>
#include <stdexcept>
#include <tuple>

namespace pluginplay {

const Plugin::default_list_type& Plugin::default_submods_from(
  const name_type& provider) const noexcept {
    static const default_list_type empty;
    auto itr = m_defaults_.find(provider);
    return itr == m_defaults_.end() ? empty : itr->second;
}

bool Plugin::operator==(const Plugin& rhs) const {
    return std::tie(m_name_, m_providers_, m_defaults_, m_modules_) ==
           std::tie(rhs.m_name_, rhs.m_providers_, rhs.m_defaults_,
                    rhs.m_modules_);
}

void Plugin::set_name(name_type name) {
    if(name.empty())
        throw std::invalid_argument(
          "Plugin names can not be empty. The empty name is reserved for the "
          "ModuleManager's own modules.");
    m_name_ = std::move(name);
}

void Plugin::set_preferred_providers(provider_list_type names) {
    std::set<name_type> seen;
    for(const auto& name : names) {
        if(name.empty())
            throw std::invalid_argument("Provider names can not be empty");
        if(!seen.insert(name).second)
            throw std::invalid_argument("Provider '" + name +
                                        "' is listed more than once");
    }
    for(const auto& [provider, records] : m_defaults_) {
        if(!seen.count(provider))
            throw std::invalid_argument(
              "Provider '" + provider +
              "' has recorded defaults, so it must stay a preferred provider");
    }
    m_providers_ = std::move(names);
}

void Plugin::default_submod_from(const name_type& provider, key_type module_key,
                                 key_type callback_key, key_type provider_key) {
    auto begin = m_providers_.begin();
    auto end   = m_providers_.end();
    if(std::find(begin, end, provider) == end)
        throw std::invalid_argument(
          "'" + provider +
          "' is not a preferred provider. Call set_preferred_providers first.");

    const auto& submods = m_modules_.at(module_key)->submods();
    if(!submods.count(callback_key))
        throw std::out_of_range("Module '" + module_key +
                                "' has no submodule with key: '" +
                                callback_key + "'");

    m_defaults_[provider].push_back(DefaultSubmod{
      std::move(module_key), std::move(callback_key), std::move(provider_key)});
}

} // namespace pluginplay
