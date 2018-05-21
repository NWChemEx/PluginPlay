#include <pybind11/pybind11.h>
namespace py = pybind11;

extern void pythonize_Module(py::module&);
extern void pythonize_ModuleManager(py::module&);

PYBIND11_MODULE(SDE, m) {
    pythonize_Module(m);
    pythonize_ModuleManager(m);
}
