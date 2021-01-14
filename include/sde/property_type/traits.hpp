#pragma once
#include <type_traits>

/** @file traits.hpp
 *
 *  Template meta-programming functions and stuff related to the PropertyType
 *  class.
 */

namespace sde {

// Forward declare Property type class so we can use it
template<typename ImplClass, typename BaseType>
class PropertyType;

namespace detail_ {

/** @brief Struct for determining what property type a property type derives
 *         from.
 *
 *  This is the primary template for the BasePropertyType class. The
 *  BasePropertyType class is charged with determining what property type a
 *  particular property type inherits from. The primary template is not defined,
 *  which in turn will result in a compile-time error if a user tries to
 *  instantiate it with a type that the primary template is not specialized for.
 *
 *  @tparam T The type we want the base property type of.
 */
template<typename T>
struct BasePropertyType;

template<typename T, typename U>
struct BasePropertyType<PropertyType<T, U>> {
    using type = U;
};

} // namespace detail_

template<typename T>
using base_property_type_t = typename detail_::BasePropertyType<T>;

template<typename T>
constexpr static bool is_base_property_type_v =
  std::is_same_v<base_property_type_t<T>, std::nullptr_t>;

} // namespace sde