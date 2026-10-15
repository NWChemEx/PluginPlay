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

#pragma once
#include <pluginplay/types.hpp>
#include <string>
#include <utilities/strings/string_tools.hpp>
#include <utility>

namespace pluginplay {

/** @brief Identifies a module in a ModuleManager.
 *
 *  A ModuleManager holds one ModuleMap per loaded plugin, plus a ModuleMap of
 *  its own. Module keys are unique within a map, but two maps may use the same
 *  key. A QualifiedKey names a module in one of two ways:
 *
 *  - key-only, *e.g.*, `"Overlap"`. The ModuleManager searches every map for
 *    the key. This is the form created implicitly from a string, so code that
 *    passes a plain module key keeps working.
 *  - qualified, *e.g.*, `{"integrals", "Overlap"}`. The ModuleManager only
 *    looks in the named plugin's map. The empty plugin name, `""`, names the
 *    ModuleManager's own map. Hence `{"", "Overlap"}` is qualified and is
 *    not the same as `"Overlap"`: it is the only way to reach the
 *    ModuleManager's own "Overlap" when a plugin also has one.
 *
 *  Like module keys and plugin names elsewhere in PluginPlay, both the plugin
 *  name and the module key are compared case-insensitively.
 */
class QualifiedKey {
public:
    /// Type used for the names of plugins
    using name_type = std::string;

    /// Type used for module keys
    using key_type = type::key;

    /** @brief Creates a key-only QualifiedKey.
     *
     *  This constructor is implicit so that a module key can be passed
     *  anywhere a QualifiedKey is expected.
     *
     *  @param[in] key The module key.
     *
     *  @throw None No throw guarantee.
     */
    QualifiedKey(key_type key) noexcept : m_key_(std::move(key)) {}

    /** @brief Creates a key-only QualifiedKey from a string literal.
     *
     *  @param[in] key The module key.
     *
     *  @throw std::bad_alloc if there is a problem copying @p key. Strong throw
     *                        guarantee.
     */
    QualifiedKey(const char* key) : QualifiedKey(key_type(key)) {}

    /** @brief Creates a QualifiedKey that names the plugin.
     *
     *  @param[in] plugin The name of the plugin the module is in. The empty
     *                    name means the ModuleManager's own modules.
     *  @param[in] key The module key.
     *
     *  @throw None No throw guarantee.
     */
    QualifiedKey(name_type plugin, key_type key) noexcept :
      m_plugin_(std::move(plugin)),
      m_key_(std::move(key)),
      m_qualified_(true) {}

    /// True if *this names a plugin and false otherwise. No throw guarantee.
    bool is_qualified() const noexcept { return m_qualified_; }

    /** @brief The name of the plugin the module is in.
     *
     *  @return The name of the plugin. Empty if *this is key-only or names the
     *          ModuleManager's own modules.
     *
     *  @throw None No throw guarantee.
     */
    const name_type& plugin() const noexcept { return m_plugin_; }

    /// The module key. No throw guarantee.
    const key_type& key() const noexcept { return m_key_; }

    /** @brief A string representation of *this.
     *
     *  @return `"<plugin>::<key>"` if *this is qualified and `"<key>"`
     *          otherwise.
     *
     *  @throw std::bad_alloc if there is a problem allocating the string.
     *                        Strong throw guarantee.
     */
    std::string to_string() const {
        return m_qualified_ ? m_plugin_ + "::" + m_key_ : m_key_;
    }

    /** @brief Determines if *this and @p rhs name the same module.
     *
     *  Two QualifiedKeys are equal if they are both key-only or both qualified,
     *  and have the same plugin name and module key (both compared
     *  case-insensitively).
     *
     *  @param[in] rhs The QualifiedKey to compare to.
     *
     *  @return True if *this equals @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const QualifiedKey& rhs) const noexcept {
        using utilities::strings::case_insensitive_equal;
        if(m_qualified_ != rhs.m_qualified_) return false;
        return case_insensitive_equal(m_plugin_, rhs.m_plugin_) &&
               case_insensitive_equal(m_key_, rhs.m_key_);
    }

    /// Negates operator==
    bool operator!=(const QualifiedKey& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// The name of the plugin the module is in
    name_type m_plugin_;

    /// The module key
    key_type m_key_;

    /// Does *this name a plugin?
    bool m_qualified_ = false;
};

} // namespace pluginplay
