#pragma once
#include "SDE/Property.hpp"
#include <pybind11/pybind11.h>

namespace SDE {
namespace detail_ {

// Primary template used to determine Args for actual instantiation
template<typename Derived, typename tuple_type>
struct PyModuleType;

// Implementation of module type trampoline
template<typename ModuleAPI, typename... Args>
struct PyModuleType<ModuleAPI, std::tuple<Args...>> : ModuleAPI {
    using return_type = typename Property<ModuleAPI>::return_type;

    return_type run(Args... args) override {
        PYBIND11_OVERLOAD_PURE(return_type, ModuleAPI, run, args...);
    }
};

template<typename ModuleAPI>
struct PyPropertyHelper {
    using base_type     = Property<ModuleAPI>;
    using return_type   = typename base_type::return_type;
    using shared_return = typename base_type::shared_return;
    using args_type     = typename base_type::args_type;

    static return_type call(base_type& prop, pybind11::args args) {
        constexpr auto nargs = std::tuple_size<args_type>::value;
        return *call_(prop, args, std::make_index_sequence<nargs>());
    }

    template<size_t... I>
    static shared_return call_(base_type& prop, pybind11::args args,
                               std::index_sequence<I...>) {
        return prop(
          args[I].cast<typename std::tuple_element<I, args_type>::type>()...);
    }
};

} // namespace detail_

template<typename ModuleAPI>
void register_module(pybind11::module& m, std::string name) {
    using args_type  = typename Property<ModuleAPI>::args_type;
    using trampoline = detail_::PyModuleType<ModuleAPI, args_type>;
    pybind11::class_<ModuleAPI, std::shared_ptr<ModuleAPI>, trampoline,
                     ModuleBase>(m, name.c_str())
      .def(pybind11::init<>())
      .def("run", &ModuleAPI::run);
}

template<typename ModuleAPI>
void register_property(pybind11::module& m, std::string name) {
    using prop_type   = Property<ModuleAPI>;
    using pyprop_type = detail_::PyPropertyHelper<ModuleAPI>;
    pybind11::class_<prop_type>(m, name.c_str())
      .def(pybind11::init<std::shared_ptr<ModuleBase>>())
      .def("__call__", &pyprop_type::call);
}

} // namespace SDE

#define DEFINE_PYTHON_PROPERTY(m, PropertyName)                 \
    SDE::register_module<PropertyName>(m, #PropertyName "API"); \
    SDE::register_property<PropertyName>(m, #PropertyName)
