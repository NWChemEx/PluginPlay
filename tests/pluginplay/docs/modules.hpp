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
