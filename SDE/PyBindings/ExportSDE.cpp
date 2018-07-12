#include "SDE/Pythonization.hpp"

namespace SDE {
// Function that registers the ModuleBase class into a Python module.
extern void pythonize_Module(pybind11::module& m);
extern void pythonize_Parameters(pybind11::module&);
extern void pythonize_ModuleManager(pybind11::module&);
PYBIND11_MODULE(SDE, m) {
    pythonize_Parameters(m);
    pythonize_Module(m);
    pythonize_ModuleManager(m);
}
} // namespace SDE
