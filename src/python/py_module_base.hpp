/*
 * Copyright 2023 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
