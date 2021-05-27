#include "load_modules.hpp"
#include "modules.hpp"

namespace sde_examples {

void load_modules(sde::ModuleManager& mm) {
    using coulombs_law_float = TemplatedCoulombsLaw<float>;
    mm.add_module<ClassicalForce>("Force");
    mm.add_module<CoulombsLaw>("Coulomb's Law");
    mm.add_module<ScreenedCoulombsLaw>("Coulomb's Law with screening");
    mm.add_module<coulombs_law_float>("Single-precision Coulomb's law");

    mm.change_submod("Force", "electric field", "Coulomb's Law");
}

} // namespace sde_examples