#include "SDE/Pythonization.hpp"

namespace SDE {
// Function that registers the ModuleBase class into a Python module.
extern void pythonize_Module(pybind11::module& m);

PYBIND11_MODULE(SDE, m) { pythonize_Module(m); }
} // namespace SDE
