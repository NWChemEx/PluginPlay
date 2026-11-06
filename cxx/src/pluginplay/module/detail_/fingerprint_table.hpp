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
#include <map>
#include <mutex>
#include <pluginplay/fields/module_input.hpp>
#include <pluginplay/types.hpp>
#include <pluginplay/utility/uuid.hpp>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace pluginplay::detail_ {

/** @brief Assigns IDs, called fingerprints, to the states of modules.
 *
 *  A module's fingerprint identifies the state of the module and of
 *  everything below it. It is the ID of the module's description: the UUID
 *  of its implementation, whether it memoizes, the fingerprints of the
 *  modules bound to its submodules, and its bound inputs. Equal descriptions
 *  get the same ID, so comparing fingerprints compares descriptions.
 *
 *  Each ModuleManager owns one table and gives it to its modules. The table
 *  holds copies of the inputs it has seen, so it must not outlive the objects
 *  those inputs need, e.g., the Python interpreter.
 */
class FingerprintTable {
public:
    /// Type of a fingerprint
    using fingerprint_type = std::string;

    /// Type of the map from submodule keys to fingerprints
    using submod_map_type = std::map<type::key, fingerprint_type>;

    /// The parts of a module's description which can be ordered
    struct Key {
        /// The UUID of the module's implementation
        std::string impl;

        /// Does the module memoize?
        bool memoizes;

        /// The fingerprints of the modules bound to the submodules
        submod_map_type submods;

        /// Orders keys lexicographically by their members
        bool operator<(const Key& rhs) const {
            return std::tie(impl, memoizes, submods) <
                   std::tie(rhs.impl, rhs.memoizes, rhs.submods);
        }
    };

    /** @brief Returns the fingerprint of a description, making one if needed.
     *
     *  @param[in] key The orderable part of the description.
     *  @param[in] inputs The inputs bound to the module.
     *
     *  @return The fingerprint of the description.
     *
     *  @throw std::bad_alloc if there is a problem storing a new description.
     *                        Strong throw guarantee.
     */
    fingerprint_type intern(Key key, const type::input_map& inputs) {
        std::lock_guard<std::mutex> guard(m_mutex_);
        auto& bucket = m_table_[std::move(key)];
        for(const auto& [bucket_inputs, id] : bucket)
            if(bucket_inputs == inputs) return id;
        // N.B. copying the inputs deep copies their values, so the table does
        // not refer to the module's values
        bucket.emplace_back(inputs, utility::generate_uuid());
        return bucket.back().second;
    }

private:
    /// Inputs and fingerprints of the descriptions with the same Key
    using bucket_type =
      std::vector<std::pair<type::input_map, fingerprint_type>>;

    /// Guards m_table_
    std::mutex m_mutex_;

    /// The fingerprints
    std::map<Key, bucket_type> m_table_;
};

} // namespace pluginplay::detail_
