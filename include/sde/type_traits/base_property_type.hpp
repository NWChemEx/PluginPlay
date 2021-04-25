#pragma once
#include "sde/property_type/property_type_fwd.hpp"
#include <type_traits> // std::is_same_v

namespace sde {
namespace detail_ {

/** @brief Struct for determining what property type a property type derives
 *         from.
 *
 *  This is the primary template for the PropertyTypeBase class. The
 *  PropertyTypeBase class is charged with determining what property type a
 *  particular property type inherits from. The primary template is not defined,
 *  which in turn will result in a compile-time error if a user tries to
 *  instantiate it with a type that the primary template is not specialized for.
 *
 *  @tparam T The type we want the base property type of.
 */
template<typename T>
struct PropertyTypeBase;

/** @brief Specializes PropertyTypeBase for when the template type parameter is
 *         a PropertyType.
 *
 *  This struct will contain a member type `type` set to the type of the base
 *  property type.
 *
 *  @tparam T Assumed to be either ModuleInput or ModuleResult.
 *  @tparam U The type of the base property type.
 */
template<typename T, typename U>
struct PropertyTypeBase<PropertyType<T, U>> {
    /// The type of the property type PropertyType<T, U> is derived from
    using type = U;
};

} // namespace detail_

/** @brief Public API for getting the property type that a property type is
 *         derived from.
 *
 *  `base_property_type_t` will be a typedef of the property type that @T
 *  derives from. If @p T is not a property type this type will fail to compile.
 *
 *  @param T The property type we want to know the base property type of.
 */
template<typename T>
using base_property_type_t = typename detail_::PropertyTypeBase<T>::type;

} // namespace sde