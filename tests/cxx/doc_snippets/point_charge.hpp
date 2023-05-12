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

#pragma once
#include <array>

namespace pluginplay_examples {

using Point = std::array<double, 3>;

struct PointCharge {
    double m_charge;
    Point m_r;
};

} // namespace pluginplay_examples

// These are down here to get this to compile (objects appearing in APIs need to
// be comparable) without complicating the tutorial
#include <pluginplay/pluginplay.hpp>

namespace pluginplay_examples {
inline bool operator==(const pluginplay_examples::PointCharge& q0,
                       const pluginplay_examples::PointCharge& q1) {
    return (q0.m_charge == q1.m_charge) && (q0.m_r == q1.m_r);
}

inline bool operator<(const pluginplay_examples::PointCharge& q0,
                      const pluginplay_examples::PointCharge& q1) {
    return std::tie(q0.m_charge, q0.m_r) < std::tie(q1.m_charge, q1.m_r);
}

} // namespace pluginplay_examples
