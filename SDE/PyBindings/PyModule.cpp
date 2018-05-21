#include "SDE/Memoization.hpp"
#include "SDE/Module.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using module_pointer = typename SDE::ModuleBase::module_pointer;

namespace SDE {
namespace detail_ {

struct PyModuleBase {
    void set_submodule(module_pointer me, std::string key, module_pointer ptr) {
        me->submodules_[key] = ptr;
    }
};

} // namespace detail_
} // namespace SDE

void pythonize_Module(pybind11::module& m) {
    pybind11::class_<SDE::ModuleBase, module_pointer>(m, "ModuleBase")
      .def(pybind11::init<>())
      .def("submodules", &SDE::ModuleBase::submodules)
      .def("metadata", &SDE::ModuleBase::metadata)
      //.def("parameters", &SDE::ModuleBase::parameters)
      .def("change_submodule", &SDE::ModuleBase::change_submodule)
      //.def("change_parameter", &SDE::ModuleBase::change_parmaeter)
      .def("locked", &SDE::ModuleBase::locked)
      .def("lock", &SDE::ModuleBase::lock)
      .def("__eq__",
           [](module_pointer lhs, module_pointer rhs) { return lhs == rhs; })
      .def("_set_submodule",
           [](module_pointer me, std::string key, module_pointer ptr) {
               SDE::detail_::PyModuleBase().set_submodule(me, key, ptr);
           });
}
