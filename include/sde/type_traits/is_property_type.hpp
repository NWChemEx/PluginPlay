#pragma once
#include "sde/property_type/property_type_fwd.hpp"
#include <type_traits> // std::is_convertible_v

namespace sde {
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
template<typename T, typename BasePT = sde::detail_::BasePropertyType>
struct IsPropertyType {
    /// If T is a property type it inherits from this type
    using base_prop_type = sde::PropertyType<T, BasePT>;

    /// This member is true if T is a property type and false otherwise
    static constexpr bool value = std::is_convertible_v<T*, base_prop_type*>;
};

} // namespace detail_

/// Helper for determining if type @p T is the type of a base property type
template<typename T>
static constexpr bool is_base_property_type_v =
  detail_::IsPropertyType<T>::value;

} // namespace sde