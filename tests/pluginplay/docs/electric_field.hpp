#pragma once
#include "point_charge.hpp"
#include <pluginplay/pluginplay.hpp>

namespace pluginplay_examples {

DECLARE_PROPERTY_TYPE(ElectricField);

PROPERTY_TYPE_INPUTS(ElectricField) {
    // Type of an std::vector filled with PointCharge instances
    using point_charge_vector = std::vector<PointCharge>;

    auto rv = pluginplay::declare_input()
                .add_field<const Point&>("r")
                .template add_field<const point_charge_vector&>("charges");

    rv.at("r").set_description(
      "The point where the electric field will be evaluated.");
    rv.at("charges").set_description(
      "The set of point charges giving rise to the electric field.");
    return rv;
}

PROPERTY_TYPE_RESULTS(ElectricField) {
    return pluginplay::declare_result().add_field<Point>("electric field at r");
}

} // namespace pluginplay_examples
