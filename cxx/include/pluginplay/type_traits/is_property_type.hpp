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
#include "pluginplay/property_type/property_type_fwd.hpp"
#include <type_traits> // std::is_convertible_v

namespace pluginplay {
namespace detail_ {

/** @brief Determines if type @p T is the type of a property type.
 *
 *  All property types are implemented via CRTP. Thus if @p T is the type of a
 *  property type it must inherit from `PropertyType<T, U>` where `U` is either:
 *
 *  - BasePropertyType if `T` is a base property type, or
 *  - another type which is itself a property type.
 *
 *  The type `IsPropertyType<T>` will contain a static member `value`, the value
 *  of which indicates whether or not @p T is a a property type.
 *
 *  @param T The type whose property-type-ness is being questioned. @p T is
 *           expected to be a clean type (i.e. no pointer, reference, or const)
 *  @param BasePT The property type that @p T might inherit from. Default is
 *                `BasePropertyType`
 */
template<typename T, typename BasePT = pluginplay::detail_::BasePropertyType>
struct IsPropertyType {
    /// If T is a property type it inherits from this type
    using base_prop_type = pluginplay::PropertyType<T, BasePT>;

    /// This member is true if T is a property type and false otherwise
    static constexpr bool value = std::is_convertible_v<T*, base_prop_type*>;
};

} // namespace detail_

/// Helper for determining if type @p T is the type of a base property type
template<typename T>
static constexpr bool is_base_property_type_v =
  detail_::IsPropertyType<T>::value;

} // namespace pluginplay
