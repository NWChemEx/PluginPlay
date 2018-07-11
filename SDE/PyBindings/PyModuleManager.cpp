#include "SDE/ModuleManager.hpp"
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
namespace py = pybind11;

namespace SDE {
using module_pointer = typename ModuleManager::module_pointer;

void pythonize_ModuleManager(py::module& m) {
    py::class_<ModuleManager>(m, "ModuleManager")
      .def(py::init<>())
      .def("__contains__", &ModuleManager::count)
      .def("insert", &ModuleManager::insert, py::keep_alive<1, 3>())
      .def("duplicate", &ModuleManager::duplicate, py::arg("old_key"),
           py::arg("new_key") = "")
      .def("at", &ModuleManager::at);
}
} // namespace SDE
