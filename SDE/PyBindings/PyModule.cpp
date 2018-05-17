#include "SDE/Memoization.hpp"
#include "SDE/Module.hpp"
#include <pybind11/pybind11.h>

void pythonize_Module(pybind11::module& m) {
    pybind11::class_<SDE::ModuleBase, std::shared_ptr<SDE::ModuleBase>>(
      m, "ModuleBase")
      .def(pybind11::init<>())
      .def("submodules", &SDE::ModuleBase::submodules)
      .def("meta_data", &SDE::ModuleBase::meta_data)
      //.def("parameters", &SDE::ModuleBase::parameters)
      .def("change_submodule", &SDE::ModuleBase::change_submodule)
      //.def("change_parameter", &SDE::ModuleBase::change_parmaeter)
      .def("locked", &SDE::ModuleBase::locked)
      .def("lock", &SDE::ModuleBase::lock);
}
