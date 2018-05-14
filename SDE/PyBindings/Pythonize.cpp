#include <pybind11/pybind11.h>
namespace py = pybind11;

extern void pythonize_ModuleManager(py::module&);

PYBIND11_MODULE(SDE, m) { pythonize_ModuleManager(m); }
