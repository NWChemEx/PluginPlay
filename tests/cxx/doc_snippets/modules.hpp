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
#include <pluginplay/pluginplay.hpp>

namespace pluginplay_examples {

DECLARE_MODULE(CoulombsLaw);
DECLARE_MODULE(ScreenedCoulombsLaw);
DECLARE_MODULE(ClassicalForce);

template<typename FloatingPointType>
DECLARE_MODULE(TemplatedCoulombsLaw);

extern template class TemplatedCoulombsLaw<float>;
extern template class TemplatedCoulombsLaw<double>;

} // namespace pluginplay_examples
