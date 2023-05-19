#pragma once
#include <pluginplay/module_base.hpp>
#include <pluginplay/python/python.hpp>
#include <pybind11/pybind11.h>

namespace pluginplay {

/** @brief Defines the interface used to write modules in Python.
 *
 *
 */
class PyModuleBase : public ModuleBase {
public:
    PyModuleBase() :
      ModuleBase(python::PythonWrapper(pybind11::object()), true) {}

    type::result_map run_(type::input_map inputs,
                          type::submodule_map submods) const override;
};

// -- Inline Implementations ---------------------------------------------------

inline type::result_map PyModuleBase::run_(type::input_map inputs,
                                           type::submodule_map submods) const {
    PYBIND11_OVERRIDE_PURE(type::result_map, ModuleBase, run_, inputs, submods);
}

} // namespace pluginplay
