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
#include <pybind11/pybind11.h>

namespace pluginplay {

// -----------------------------------------------------------------------------
// -- Type factorization used throughout the Python component
// -----------------------------------------------------------------------------

/// Type of a C++ handle to a Python module
using py_module_type = pybind11::module_;

/// Type of a reference to an object of type py_module_type
using py_module_reference = py_module_type&;

/// Type of Python object binding for a C++ class of type @p T
template<typename... T>
using py_class_type = pybind11::class_<T...>;

// -----------------------------------------------------------------------------
// -- Declarations of exports for top-level classes
// -----------------------------------------------------------------------------

void export_module_manager(py_module_reference m);
void export_submodule_request(py_module_reference m);
void export_printing(py_module_reference m);

} // namespace pluginplay
