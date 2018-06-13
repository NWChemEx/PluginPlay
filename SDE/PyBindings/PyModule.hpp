#pragma once
#include "SDE/ModuleHelpers.hpp"
#include <pybind11/pybind11.h>

/** @file PyModule.hpp Contains functions and macros for helping one make
 * property types accessible to Python.
 */

namespace SDE {
namespace detail_ {

// Primary template used to determine Args for actual instantiation
template<typename Derived, typename tuple_type>
struct PyPropertyType;

/**
 * @brief This is the class that trampolines a property type so that it can be
 *        called from Python.
 *
 * This class allows us to implement a module in Python of property type @p
 * PropertyType.
 *
 * @tparam PropertyType the type of the class defining the API for the target
 *         property type.  Must satisfy the concept of property type.
 * @tparam Args The types of the arguments to @p PropertyType's run function.
 */
template<typename PropertyType, typename... Args>
struct PyPropertyType<PropertyType, std::tuple<Args...>> : PropertyType {
    /// The type returned by PropertyType::run
    using return_type = detail_::RunDetails_return_type<PropertyType>;

    /// A shared_ptr to the return
    using shared_return = std::shared_ptr<return_type>;

    /// The type of a pointer to ModuleBase
    using module_ptr = typename PropertyType::module_pointer;

    /**
     * @brief This function will actually run a module.
     *
     * Assuming the user's Python looks like:
     *
     * ```.py
     * mod = ModuleBase # Some module, passed by ModuleBase
     * result = mod.run_as(PropertyType, args...)
     * ```
     *
     * This function will then use PropertyType as if
     *
     * @param real
     * @param args
     * @return
     */
    static return_type run_as_impl(module_ptr me, pybind11::args args) {
        constexpr auto nargs = sizeof...(Args);
        return *call_(me, args, std::make_index_sequence<nargs>());
    }

    return_type run(Args... args) override {
        PYBIND11_OVERLOAD_PURE(return_type, PropertyType, run, args...);
    }

private:
    template<std::size_t... I>
    static shared_return call_(module_ptr me, pybind11::args args,
                               std::index_sequence<I...>) {
        return me->template run_as<PropertyType>(args[I].cast<Args>()...);
    }
};

} // namespace detail_

template<typename PropertyType>
void register_property_type(pybind11::module& m, std::string name) {
    using args_type   = detail_::RunDetails_args_type<PropertyType>;
    using trampoline  = detail_::PyPropertyType<PropertyType, args_type>;
    using shared_prop = std::shared_ptr<PropertyType>;
    pybind11::class_<PropertyType, shared_prop, trampoline, ModuleBase>(
      m, name.c_str())
      .def(pybind11::init<>())
      .def("run_as_impl",
           [](shared_prop me, pybind11::args args) {
               return trampoline::run_as_impl(me, args);
           })
      .def("run", &PropertyType::run);
}

template<typename ModuleType, typename PropertyType>
void register_module(pybind11::module& m, std::string name) {
    using shared_module = std::shared_ptr<ModuleType>;
    pybind11::class_<ModuleType, PropertyType, shared_module>(m, name.c_str())
      .def(pybind11::init<>())
      .def("run", &ModuleType::run);
}

} // namespace SDE
