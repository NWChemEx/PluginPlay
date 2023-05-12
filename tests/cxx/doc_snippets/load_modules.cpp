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

#include "load_modules.hpp"
#include "modules.hpp"

namespace pluginplay_examples {

void load_modules(pluginplay::ModuleManager& mm) {
    using coulombs_law_float = TemplatedCoulombsLaw<float>;
    mm.add_module<ClassicalForce>("Force");
    mm.add_module<CoulombsLaw>("Coulomb's Law");
    mm.add_module<ScreenedCoulombsLaw>("Coulomb's Law with screening");
    mm.add_module<coulombs_law_float>("Single-precision Coulomb's law");

    mm.change_submod("Force", "electric field", "Coulomb's Law");
}

} // namespace pluginplay_examples
