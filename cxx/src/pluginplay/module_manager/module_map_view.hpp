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
#include <pluginplay/module_manager/module_map.hpp>

namespace pluginplay {

/** @brief A read-only view of a ModuleMap, for the Python bindings.
 *
 *  C++ can hand out a `const ModuleMap&`, but Python has no const, so a
 *  ModuleMap returned to Python could be changed through ModuleMap's
 *  bindings, *e.g.*, have modules added that never receive the
 *  ModuleManager's runtime and caches. Functions that return a read-only
 *  ModuleMap to Python return a ModuleMapView instead, which only exposes the
 *  read-only members.
 *
 *  The view does not own the map; the bindings keep the map's owner alive for
 *  as long as the view exists. The modules themselves can still be changed
 *  (*e.g.*, their inputs), just as with ModuleManager.at, and at returns them
 *  with shared ownership.
 */
class ModuleMapView {
public:
    /// Type used for module keys
    using key_type = ModuleMap::key_type;

    /// Type of a container of module keys
    using key_container_type = ModuleMap::key_container_type;

    /// Creates a view of @p map. No throw guarantee.
    explicit ModuleMapView(const ModuleMap& map) noexcept : m_map_(&map) {}

    /// The map *this views. No throw guarantee.
    const ModuleMap& map() const noexcept { return *m_map_; }

    /// Forwards to ModuleMap::count
    type::size count(const key_type& key) const noexcept {
        return m_map_->count(key);
    }

    /// Forwards to ModuleMap::size
    type::size size() const noexcept { return m_map_->size(); }

    /// Forwards to ModuleMap::empty
    bool empty() const noexcept { return m_map_->empty(); }

    /// Forwards to ModuleMap::keys
    key_container_type keys() const { return m_map_->keys(); }

    /// Forwards to ModuleMap::at
    ModuleMap::module_pointer at(const key_type& key) const {
        return m_map_->at(key);
    }

    /// Forwards to ModuleMap::has_submod
    bool has_submod(const key_type& module_key,
                    const key_type& callback_key) const {
        return m_map_->has_submod(module_key, callback_key);
    }

    /// True if the viewed maps are equal
    bool operator==(const ModuleMapView& rhs) const {
        return *m_map_ == *rhs.m_map_;
    }

    /// Negates operator==
    bool operator!=(const ModuleMapView& rhs) const { return !(*this == rhs); }

private:
    /// The map *this views
    const ModuleMap* m_map_;
};

} // namespace pluginplay
