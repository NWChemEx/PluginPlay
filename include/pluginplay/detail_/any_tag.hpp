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
