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

#include "drivers.hpp"
#include "force.hpp"
#include <limits> // for std::numeric_limits

namespace pluginplay_examples {

using force_type = Force;

static constexpr auto driver_desc = R"(
Classical Force With an Optionally Screened Electric Field
---------------------------------------------------------

Computes the force on a charged particle with the "Classical Force" module.
The "screen" input chooses whether the electric field is screened.
)";

DRIVER_CTOR(ForceDriver) {
    description(driver_desc);
    satisfies_property_type<force_type>();
    add_submodule<force_type>("force").set_description(
      "The module that computes the force");

    add_input<bool>("screen")
      .set_description("Should the electric field be screened?")
      .set_default(false);
    add_input<double>("threshold")
      .set_description("maximum ||r-r_i|| for contributing to electric field")
      .set_default(std::numeric_limits<double>::max());

    // Use the screened electric field, and its threshold, if "screen" is true
    when("screen", true)
      .set_submod("Classical Force", "electric field",
                  "Coulomb's Law with screening")
      .forward_input("Coulomb's Law with screening", "threshold", "threshold")
      .otherwise()
      .set_submod("Classical Force", "electric field", "Coulomb's Law");
}

DRIVER_RUN(ForceDriver) {
    const auto& [q, m, a, charges] = force_type::unwrap_inputs(inputs);
    auto F = submods.at("force").run_as<force_type>(q, m, a, charges);

    auto rv = results();
    return force_type::wrap_results(rv, F);
}

} // namespace pluginplay_examples
