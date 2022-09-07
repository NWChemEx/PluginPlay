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
#include <type_traits>

namespace pluginplay::detail_ {

// Enumerates template arguments that can be used to specialize AnyWrapper
enum AnyTag { Input, Output, General };

template<AnyTag T>
struct is_input_type : std::false_type {};
template<>
struct is_input_type<AnyTag::Input> : std::true_type {};
template<>
struct is_input_type<AnyTag::General> : std::true_type {};
template<AnyTag T>
struct is_output_type : std::false_type {};
template<>
struct is_output_type<AnyTag::Output> : std::true_type {};
template<>
struct is_output_type<AnyTag::General> : std::true_type {};

template<AnyTag T>
static constexpr bool is_input_type_v = is_input_type<T>::value;
template<AnyTag T>
static constexpr bool is_output_type_v = is_output_type<T>::value;

template<AnyTag T>
using enable_if_input_type_t = std::enable_if_t<is_input_type_v<T>>;

template<AnyTag T>
using enable_if_output_type_t = std::enable_if_t<is_output_type_v<T>>;
} // namespace pluginplay::detail_
