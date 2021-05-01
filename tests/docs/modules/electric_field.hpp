#pragma once
#include "point_charge.hpp"
#include <sde/sde.hpp>

namespace sde_examples {

DECLARE_PROPERTY_TYPE(ElectricField);

PROPERTY_TYPE_INPUTS(ElectricField) {
    // Type of an std::vector filled with PointCharge instances
    using point_charge_vector = std::vector<PointCharge>;

    return sde::declare_input()
      .add_field<Point>("r")
      .template add_field<point_charge_vector>("charges");
}

PROPERTY_TYPE_RESULTS(ElectricField) {
    return sde::declare_result().add_field<Point>("electric field at r");
}

} // namespace sde_examples