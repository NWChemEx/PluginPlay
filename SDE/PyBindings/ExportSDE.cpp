#include <pybind11/pybind11.h>
namespace py = pybind11;

namespace SDE {
extern void pythonize_Parameters(py::module&);

PYBIND11_MODULE(SDE, m) { pythonize_Parameters(m); }
} // namespace SDE
