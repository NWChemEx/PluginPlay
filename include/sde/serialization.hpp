#pragma once
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/details/traits.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

namespace sde {
template<typename T, typename Archive>
using is_serializable = cereal::traits::is_output_serializable<T, Archive>;
template<typename Archive>
using get_input_from_output =
  cereal::traits::detail::get_input_from_output<Archive>;
template<typename T, typename Archive>
using is_deserializable = cereal::traits::is_input_serializable<T, Archive>;
template<typename Archive>
using get_output_from_input =
  cereal::traits::detail::get_output_from_input<Archive>;
} // namespace sde