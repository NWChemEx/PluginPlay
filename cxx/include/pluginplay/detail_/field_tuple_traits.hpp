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
#include "pluginplay/types.hpp"
#include <array>
#include <tuple>
#include <utility>

namespace pluginplay {

/// Forward declare FieldTuple so we can use the type
template<typename ElementType, typename... FieldTypes>
class FieldTuple;

namespace detail_ {

/** @brief Responsible for working out the types to use in a FieldTuple instance
 *
 *  This class serves largely to decouple the FieldTuple class from the process
 *  of determining its types.
 *
 *  @tparam ElementType The type used to hold the actual input or result values.
 *                      It is assumed to be either ModuleInput or ModuleResult.
 *  @tparam FieldTypes The types of the fields in the order they are accepted or
 *                     returned. Can be 0 or more types.
 */
template<typename ElementType, typename... FieldTypes>
struct FieldTupleTraits {
    /// Type resulting from adding a new field of type @p T to this FieldTuple
    template<typename T>
    using new_field_tuple = FieldTuple<ElementType, FieldTypes..., T>;

    /// The number of fields in the FieldTuple
    static constexpr auto nfields = sizeof...(FieldTypes);

    /// A tuple containing the types of the fields
    using tuple_of_fields = std::tuple<FieldTypes...>;

    /// The type of the object used to retrieve a field
    using key_type = type::key;

    /// The type of the object used to hold a field
    using mapped_type = ElementType;

    /// The type used to store the field name, field pairs
    using value_type = std::pair<key_type, mapped_type>;

    /// Type of an array holding @p N fields
    template<type::size N>
    using field_array = std::array<value_type, N>;

    /// The type of a read/write iterator over the fields in a FieldTuple
    using iterator = typename field_array<nfields>::iterator;

    /// The type of a read-only iterator over the fields in a FieldTuple
    using const_iterator = typename field_array<nfields>::const_iterator;
}; // class FieldTypeTraits

} // namespace detail_
} // namespace pluginplay
