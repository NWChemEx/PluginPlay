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

#include "electric_field.hpp"
#include "modules.hpp"
#include <numeric> // for std::inner_product

namespace pluginplay_examples {

static constexpr auto module_desc = R"(
Electric Field From Coulomb's Law
---------------------------------

This module computes the electric field of a series of point charges using
Coulomb's law according to:

.. math::

   \vec{E}(\vec{r}) = \sum_{i=1}^N
                      \frac{q_i \hat{r}_i}{||\vec{r} - \vec{r}_i||^2}
)";

MODULE_CTOR(CoulombsLaw) {
    description(module_desc);
    satisfies_property_type<ElectricField>();
}

MODULE_RUN(CoulombsLaw) {
    const auto& [r, charges] = ElectricField::unwrap_inputs(inputs);

    // This will be the value of the electric field
    Point E{0.0, 0.0, 0.0};

    // This loop fills in E
    for(const auto& charge : charges) {
        auto q   = charge.m_charge;
        auto& ri = charge.m_r;

        // Magnitude of r_i
        auto ri2    = std::inner_product(ri.begin(), ri.end(), ri.begin(), 0.0);
        auto mag_ri = std::sqrt(ri2);

        // ||r - r_i||**2
        Point rij(r);
        for(std::size_t i = 0; i < 3; ++i) rij[i] -= charge.m_r[i];
        auto rij2 =
          std::inner_product(rij.begin(), rij.end(), rij.begin(), 0.0);

        for(std::size_t i = 0; i < 3; ++i) E[i] += q * ri[i] / (mag_ri * rij2);
    }

    auto rv = results();
    return ElectricField::wrap_results(rv, E);
}

} // namespace pluginplay_examples
