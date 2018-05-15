#pragma once
#include <pybind11/pybind11.h>

namespace SDE {
namespace detail_ {

// Forward declare PyModuleBaseImpl for next typedef
template<typename ReturnType, typename... Args>
class PyModuleBaseImpl;

// This typedef is the class our Python ModuleBaseImpl trampoline inherits from
template<typename ReturnType, typename... Args>
using PyModuleBaseImplBase =
  ModuleBaseImpl<PyModuleBaseImpl<ReturnType, Args...>, ReturnType, Args...>;

// This is the actual trampoline class
template<typename ReturnType, typename... Args>
class PyModuleBaseImpl : public PyModuleBaseImplBase<ReturnType, Args...> {
    // Typedef of this class for brevity
    using my_type = PyModuleBaseImpl<ReturnType, Args...>;
    // Typedef of the base class for brevity
    using base_type = ModuleBaseImpl<my_type, ReturnType, Args...>;

public:
    //    cost_type cost(py::args args) override{
    //        PYBIND11_OVERLOAD(
    //          py::object,
    //          ModuleBaseImpl<PyModuleBaseImpl, py::object, py::args>,
    //          run_,
    //          args
    //        );
    //    }

    /// Bindings for the run function
    ReturnType run_(Args... args) override {
        PYBIND11_OVERLOAD_PURE(ReturnType, base_type, run_, args...);
    }
};

/// Class for making run_ public so
template<typename ReturnType, typename... Args>
class ModuleBasePublicist : public PyModuleBaseImplBase<ReturnType, Args...> {
public:
    using PyModuleBaseImplBase<ReturnType, Args...>::run_;
};

} // namespace detail_
} // namespace SDE

template<typename ReturnType, typename... Args>
void pythonize_module_type(pybind11::module& m, const char* name) {
    using ModuleType  = SDE::detail_::PyModuleBaseImpl<ReturnType, Args...>;
    using ModBaseImpl = SDE::ModuleBaseImpl<ModuleType, ReturnType, Args...>;
    using Publicist   = SDE::detail_::ModuleBasePublicist<ReturnType, Args...>;

    pybind11::class_<ModBaseImpl, ModuleType>(m, name)
      .def(pybind11::init<>())
      .def("__call__",
           [](ModBaseImpl& mod, Args... args) { return *(mod(args...)); })
      .def("run_", &Publicist::run_);
}
