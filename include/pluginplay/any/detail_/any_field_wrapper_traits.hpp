#pragma once
#include <type_traits>

namespace pluginplay::any::detail_ {

// Forward declare for TMP purposes
template<typename T>
class AnyFieldWrapper;

/** @brief Primary template for deducing if @p T is a specialization of
 *         AnyFieldWrapper.
 *
 *  We use the templated class is_any_field_wrapper to determine if its template
 *  parameter is a specialization of AnyFieldWrapper. This is the primary
 *  template and it is selected when @p T is not an AnyFieldWrapper
 *  specialization.
 *
 *  @tparam T the type we are inspecting.
 */
template<typename T>
struct is_any_field_wrapper : std::false_type {};

/** @brief Specialization of is_any_field_wrapper for when the template
 *         parameter is an AnyFieldWrapper.
 *
 *  This implementation of AnyFieldWrapper is choosen when @p T is a
 *  specialization of AnyFieldWrapper.
 *
 *  @tparam T The value of the AnyFieldWrapper template parameter. For the
 *            purposes of this trait there are no resctictions on @p T.
 */
template<typename T>
struct is_any_field_wrapper<AnyFieldWrapper<T>> : std::true_type {};

/** @brief Disables a function when @p U is an AnyFieldWrapper
 *         specialization.
 *
 *  This type is used to disable a function, via SFINAE, if the type of the
 *  input is a specialization of AnyFieldWrapper. It is primarily used to
 *  dispatch to the copy ctor rather than the value ctor.
 *
 *  @tparam U The type the function is templated on
 *  @tparam V the type to return if @p U isn't a specialization of
 *            AnyFieldWrapper
 */
template<typename U, typename V = void>
using disable_if_any_field_wrapper_t =
  std::enable_if_t<!is_any_field_wrapper<U>::value, V>;

} // namespace pluginplay::any::detail_
