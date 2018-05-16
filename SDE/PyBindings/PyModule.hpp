#pragma once
#include <pybind11/pybind11.h>

namespace SDE {
namespace detail_ {

// This is the actual trampoline class
template<typename Derived, typename ReturnType, typename... Args>
struct PyModuleType : Derived {
    ReturnType run_(Args... args) override {
        PYBIND11_OVERLOAD_PURE(ReturnType, Derived, run_, args...);
    }
};

template<typename Derived>
struct run_Publicist : Derived {
    using Derived::run_;
};

} // namespace detail_

template<typename Derived, typename ReturnType, typename... Args>
void pythonize_module_type(pybind11::module& m, const char* name) {
    using ModType   = detail_::PyModuleType<Derived, ReturnType, Args...>;
    using Publicist = detail_::run_Publicist<Derived>;
    pybind11::class_<Derived, ModType>(m, name)
      .def(pybind11::init<>())
      .def("clone", &Derived::clone)
      .def("__call__",
           [](Derived& mod, Args... args) { return *(mod(args...)); },
           pybind11::return_value_policy::copy)
      .def("run_", &Publicist::run_);
};

template<typename ModType, typename... Args>
void pythonize_property(pybind11::module& m, const char* name) {
    using property_type = PropertyBase<ModType>;

    pybind11::class_<property_type>(m, name)
      .def(pybind11::init([](ModuleBase& mod) {
          return std::make_unique<property_type>(std::move(mod.clone()));
      }))
      .def("__call__",
           [](ModType& mod, Args... args) { return *(mod(args...)); });
}

} // namespace SDE
