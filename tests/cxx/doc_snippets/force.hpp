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
#include "point_charge.hpp"
#include <pluginplay/pluginplay.hpp>

namespace pluginplay_examples {

DECLARE_PROPERTY_TYPE(Force);

PROPERTY_TYPE_INPUTS(Force) {
    // Type of an std::vector filled with PointCharge instances
    using point_charge_vector = std::vector<PointCharge>;

    return pluginplay::declare_input()
      .add_field<PointCharge>("q")
      .template add_field<double>("mass")
      .template add_field<Point>("acceleration")
      .template add_field<point_charge_vector>("charges");
}

PROPERTY_TYPE_RESULTS(Force) {
    return pluginplay::declare_result().add_field<Point>("Force field at r");
}

} // namespace pluginplay_examples
