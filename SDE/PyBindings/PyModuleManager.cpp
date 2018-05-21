#include "SDE/ModuleManager.hpp"
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
namespace py = pybind11;
using namespace SDE;

void pythonize_ModuleManager(py::module& m) {
    py::class_<ModuleManager>(m, "ModuleManager")
      .def(py::init<>())
      .def("__contains__", &ModuleManager::count)
      .def("insert", &ModuleManager::insert)
      .def("duplicate", &ModuleManager::duplicate, py::arg("old_key"),
           py::arg("new_key") = "")
      .def("at", &ModuleManager::at);
}
