#include "electric_field.hpp"
#include "force.hpp"
#include "modules.hpp"

namespace pluginplay_examples {

using efield_type = ElectricField;
using force_type  = Force;

static constexpr auto module_desc = R"(
Computes the force of a moving charged particle as it moves through the electric
field arising from a set of point charges. Assuming the particle has a mass
:math:`m`, a charge :math:`q`, and an acccleration :math:`\vec{a}`, the force
:math:`\vec{F}` is given by:

.. math::

   \vec{F}(\vec{r}) = m\vec{a}\left(\vec{r}\right) +
                      q\vec{E}\left(\vec{r}\right)
)";

MODULE_CTOR(ClassicalForce) {
    description(module_desc);
    satisfies_property_type<force_type>();

    add_submodule<efield_type>("electric field")
      .set_description(
        "Used to compute the electric field of the point charges");
}

MODULE_RUN(ClassicalForce) {
    const auto& [q, m, a, charges] = force_type::unwrap_inputs(inputs);

    // This will be the value of the force
    Point F{0.0, 0.0, 0.0};

    auto [E] = submods.at("electric field").run_as<efield_type>(q.m_r, charges);

    for(std::size_t i = 0; i < 3; ++i) { F[i] = m * a[i] + q.m_charge * E[i]; }

    auto rv = results();
    return force_type::wrap_results(rv, F);
}

} // namespace pluginplay_examples
