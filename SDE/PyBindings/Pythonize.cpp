#include <pybind11/pybind11.h>
namespace py = pybind11;

namespace SDE {
extern void pythonize_Module(py::module&);

PYBIND11_MODULE(SDE, m) { pythonize_Module(m); }
} // namespace SDE