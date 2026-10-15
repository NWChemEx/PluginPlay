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
#include <cctype>
#include <pluginplay/plugin/plugin_class.hpp>
#include <set>
#include <stdexcept>
#include <tuple>
#include <utilities/strings/string_tools.hpp>

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

bool Plugin::is_valid_name(const name_type& name) noexcept {
    if(name.empty()) return false;
    const auto first = static_cast<unsigned char>(name.front());
    if(!std::isalpha(first) && first != '_') return false;
    return std::all_of(name.begin(), name.end(), [](unsigned char c) {
        return std::isalnum(c) || c == '_';
    });
}

void Plugin::set_name(name_type name) {
    if(name.empty())
        throw std::invalid_argument(
          "Plugin names can not be empty. The empty name is reserved for the "
          "ModuleManager's own modules.");
    if(!is_valid_name(name))
        throw std::invalid_argument(
          "'" + name +
          "' is not a valid plugin name. Plugin names must be identifiers: "
          "letters, digits, and underscores, not starting with a digit.");
    m_name_ = std::move(name);
}

void Plugin::set_preferred_providers(provider_list_type names) {
    // Plugin names are case-insensitive
    using name_less = utilities::CaseInsensitiveMap<name_type>::key_compare;
    std::set<name_type, name_less> seen;
    for(const auto& name : names) {
        if(!is_valid_name(name))
            throw std::invalid_argument(
              "'" + name +
              "' is not a valid plugin name, so it can not be a provider. "
              "Plugin names must be identifiers.");
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
    auto begin   = m_providers_.begin();
    auto end     = m_providers_.end();
    auto matches = [&provider](const name_type& name) {
        return utilities::strings::case_insensitive_equal(name, provider);
    };
    if(std::find_if(begin, end, matches) == end)
        throw std::invalid_argument(
          "'" + provider +
          "' is not a preferred provider. Call set_preferred_providers first.");

    const auto& submods = m_modules_.at(module_key)->submods();
    if(!submods.count(callback_key))
        throw std::out_of_range("Module '" + module_key +
                                "' has no submodule with key: '" +
                                callback_key + "'");

    auto [itr, inserted] = m_defaults_.try_emplace(provider);
    try {
        itr->second.push_back(DefaultSubmod{std::move(module_key),
                                            std::move(callback_key),
                                            std::move(provider_key)});
    } catch(...) {
        if(inserted) m_defaults_.erase(itr);
        throw;
    }
}

} // namespace pluginplay
