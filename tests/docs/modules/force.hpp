#pragma once
#include "point_charge.hpp"
#include <sde/sde.hpp>

namespace sde_examples {

DECLARE_PROPERTY_TYPE(Force);

PROPERTY_TYPE_INPUTS(Force) {
    // Type of an std::vector filled with PointCharge instances
    using point_charge_vector = std::vector<PointCharge>;

    return sde::declare_input()
      .add_field<PointCharge>("q")
      .template add_field<double>("mass")
      .template add_field<Point>("velocity")
      .template add_field<point_charge_vector>("charges");
}

PROPERTY_TYPE_RESULTS(Force) {
    return sde::declare_result().add_field<Point>("Force field at r");
}

} // namespace sde_examples