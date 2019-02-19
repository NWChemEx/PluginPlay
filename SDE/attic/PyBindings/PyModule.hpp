#pragma once
#include "SDE/attic/Module.hpp"
#include "SDE/attic/ModuleHelpers.hpp"
#include <pybind11/pybind11.h>

/** @file PyModule.hpp Contains functions and macros for helping one make
 * property types accessible to Python.
 */

namespace SDE {
namespace detail_ {

// Primary template used to determine Args for actual instantiation (below)
template<typename Derived, typename tuple_type>
struct PyPropertyType;

/**
 * @brief This is the class that trampolines a property type so that it can be
 *        called from Python.
 *
 * This class allows us to implement a module in Python of property type @p
 * PropertyType.  It is a specialization of the PyPropertyType to the case where
 * @p tuple_type is an std::tuple.
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
     * @brief This function will actually run a module.  Using a visitor pattern
     * to deduce the type to downcast to.
     *
     * @warning the underscore prefixing this function's name is Python
     * convention for a private member function.  This function is the C++
     * implementation of the corresponding Python class's private member.  It
     * must be public, in C++, to be exposed to Python.  This function should
     * not be called directly from within either C++ or Python.
     *
     * Here's the plan.  Say we have a module, A, and we want to run it as
     * property type P.  In Python we call A's `run_as` member providing it P
     * (we actually provide it the class name, not an instance of the class)
     * and the actual input arguments to the module `*args`.  Next, the Python
     * bindings for A's `run_as` call `_run_as_impl` on P, providing it A and
     * `*args`.  Within `_run_as_impl` we know the type to cast A down to (it's
     * the C++ type P is binding) and simply provide that information to A's
     * `run_as` member.  Basically, we treat A as a visitor to P.
     *
     * @param me The ModuleBase instance to actually call.  Note the module
     *        whose `_run_as_impl` is being called is a dummy module, @p me is
     *        of the same type, but needs downcast.
     * @param args The arguments to be forwarded to @p me.
     * @return The result of running the module.
     */
    static return_type _run_as_impl(module_ptr me, pybind11::args args) {
        constexpr auto nargs = sizeof...(Args);
        return *call_(me, args, std::make_index_sequence<nargs>());
    }

    /// The function to implement to develop a module in Python
    return_type run(Args... args) override {
        PYBIND11_OVERLOAD_PURE(return_type, PropertyType, run, args...);
    }

private:
    /// TODO: use std::apply instead
    template<std::size_t... I>
    static shared_return call_(module_ptr me, pybind11::args args,
                               std::index_sequence<I...>) {
        return me->template run_as<PropertyType>(args[I].cast<Args>()...);
    }
};

} // namespace detail_

/** @brief Helper function for registering a new property type with Python.
 *
 * @tparam PropertyType Abstract base class defining the property type's API.
 *         Must satisfy the concept of property type.
 * @param m The Python module to add the property type to.
 * @param name The (case-sensitive) symbol to use to create an instance of this
 *        class in Python.
 */
template<typename PropertyType>
void register_property_type(pybind11::module& m, std::string name) {
    using args_type   = detail_::RunDetails_args_type<PropertyType>;
    using trampoline  = detail_::PyPropertyType<PropertyType, args_type>;
    using shared_prop = std::shared_ptr<PropertyType>;
    pybind11::module::import("SDE");
    pybind11::class_<PropertyType, shared_prop, trampoline, ModuleBase>(
      m, name.c_str())
      .def(pybind11::init<>())
      .def("_run_as_impl",
           [](shared_prop me, pybind11::args args) {
               return trampoline::_run_as_impl(me, args);
           })
      .def("run", &PropertyType::run);
}

/** @brief Helper function for registering a new module with Python.
 *
 * @tparam PropertyType Property type this module implements.
 * @param m The Python module to add the property type to.
 * @param name The (case-sensitive) symbol to use to create an instance of this
 *        class in Python.
 */
template<typename ModuleType, typename PropertyType>
void register_module(pybind11::module& m, std::string name) {
    using shared_module = std::shared_ptr<ModuleType>;
    pybind11::module::import("SDE");
    pybind11::class_<ModuleType, PropertyType, shared_module>(m, name.c_str())
      .def(pybind11::init<>())
      .def("run", &ModuleType::run);
}

} // namespace SDE
