#pragma once
#include "pluginplay/any/detail_/any_field_wrapper.hpp"
#include "pluginplay/any/detail_/any_input_base.hpp"

namespace pluginplay::detail_ {

// Forward declare for TMP purposes
template<typename T>
class AnyInputWrapper;

/** @brief Primary template for deducing if @p T is a specialization of
 *         AnyInputWrapper.
 *
 *  We use the templated class is_any_input_wrapper to determine if its template
 *  parameter is a specialization of AnyInputWrapper. This is the primary
 *  template and it is selected when @p T is not an AnyInputWrapper
 *  specialization.
 *
 *  @tparam T the type we are inspecting.
 */
template<typename T>
struct is_any_input_wrapper : std::false_type {};

/** @brief Specialization of is_any_input_wrapper for when the template
 *         parameter is an AnyInputWrapper.
 *
 *  This implementation of AnyInputWrapper is choosen when @p T is a
 *  specialization of AnyInputWrapper.
 *
 *  @tparam T The value of the AnyInputWrapper template parameter. For the
 *            purposes of this trait there are no resctictions on @p T.
 */
template<typename T>
struct is_any_input_wrapper<AnyInputWrapper<T>> : std::true_type {};

template<typename T>
class AnyInputWrapper : public AnyFieldWrapper<T, AnyInputBase> {
private:
    /// Type this class derives from
    using base_type = AnyFieldWrapper<T, AnyInputBase>;

    /// Type of this class, inluding the template parameter
    using my_type = AnyInputWrapper<T>;

    /** @brief Disables a function when @p U is an AnyInputWrapper
     *         specialization.
     *
     *  This type is used to disable a function, via SFINAE, if the type of the
     *  input is a specialization of AnyInputWrapper. It is primarily used to
     *  dispatch to the copy ctor rather than the value ctor.
     *
     *  @tparam U The type the function is templated on
     *  @tparam V the type to return if @p U isn't a specialization of
     *            AnyInputWrapper
     */
    template<typename U, typename V = void>
    using disable_if_any_input_wrapper_t =
      std::enable_if<!is_any_input_wrapper<U>, V>;

public:
    template<typename U, typename = disable_if_any_input_wrapper_t<U>>
    AnyInputWrapper(U&& value2wrap);

private:
    void hash_(hasher_reference h) const override;
};

template<typename T>
template<typename U, typename>
AnyInputWrapper<T>::AnyInputWrapper(U&& value2wrap) {}

} // namespace pluginplay::detail_
