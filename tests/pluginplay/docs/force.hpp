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
