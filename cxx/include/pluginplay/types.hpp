/*
 * Copyright 2022 NWChemEx-Project
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
#include <cstring>
#include <string>
#include <typeindex>
#include <utilities/containers/case_insensitive_map.hpp>

///@{
/** @name Forward declarations
 *
 *  The types in this section are only forward declarations. They are here so
 *  we can define some of our common typedefs. They are forward declarations so
 *  that every file that pulls in this header (which is basically all of the
 *  pluginplay) does not also have to pull in the header files for all of these
 * types. If a file uses one of the types that uses a forward declaration, then
 * that file is responsible for including the correct header file for the
 * forwarded type.
 */

namespace pluginplay {
namespace any {
class AnyField;
} // namespace any

class ModuleInput;

class ModuleResult;

class SubmoduleRequest;

} // namespace pluginplay
///@}

/** @brief  Holds typedefs of types commonly used throughout the pluginplay.
 *
 *  Although C++11's auto helps a lot, in general it is best if you declare
 *  types once and use typedefs throughout a library. In the event that you
 *  need to change a type, this practice makes it so that you only have to
 *  change the type in one place. Placing the typedefs in a nested namespace
 *  avoids polluting the global namespace with our typedefs if the user decides
 *  to do `using namespace std;` or something similar.
 */
namespace pluginplay::type {

/// The type of our type-erased holder class
using any = any::AnyField;

/// The type of metadata holding a description
using description = std::string;

/// Type of a map containing inputs
using input_map = utilities::CaseInsensitiveMap<ModuleInput>;

/// Key type for any pluginplay class behaving like an associative container
using key = std::string;

/// Type of a map containing results
using result_map = utilities::CaseInsensitiveMap<ModuleResult>;

/// Type of the RTTI
using rtti = std::type_index;

/// True if @p lhs and @p rhs denote the same C++ type.
///
/// pybind11 extension modules are commonly loaded with independent RTTI
/// state across shared objects (e.g. via dlopen with hidden symbol
/// visibility), so comparing two std::type_index for the identical C++
/// type can spuriously return false when the two sides originate from
/// different DSOs. Falling back to a name-string comparison works around
/// this libc++/Python cross-DSO quirk.
inline bool rtti_equal(const rtti& lhs, const rtti& rhs) {
    return lhs == rhs || std::strcmp(lhs.name(), rhs.name()) == 0;
}

/// Type of a natural number, including zero
using size = std::size_t;

/// Type of a map containing submodules
using submodule_map = utilities::CaseInsensitiveMap<SubmoduleRequest>;

} // namespace pluginplay::type
