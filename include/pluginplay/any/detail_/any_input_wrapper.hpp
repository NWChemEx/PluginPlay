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
      std::enable_if<!is_any_input_wrapper<U>::value, V>;

    /// Are we wrapping a const reference
    static constexpr bool wrap_const_ref_v =
      base_type::template is_const_ref<T>;

    /// Type we're wrapping if we need to use a reference wrapper
    using ref_wrapper_t = std::reference_wrapper<const std::decay_t<T>>;

public:
    /// Pointer to the base class of this hierarchy
    using typename base_type::input_base_pointer;

    /// How the hasher object is passed
    using typename base_type::hasher_reference;

    /// This is the type of the object actually in the std::any
    using wrapped_type = std::conditional_t<wrap_const_ref_v, ref_wrapper_t, T>;

    template<typename U, typename = disable_if_any_input_wrapper_t<U>>
    AnyInputWrapper(U&& value2wrap);

    AnyInputWrapper(const AnyInputWrapper& other) = default;

protected:
    input_base_pointer clone_() const override;

    bool are_equal_(const AnyFieldBase& rhs) const noexcept override;

private:
    bool storing_reference_() const noexcept override;

    bool storing_const_value_() const noexcept override;

    template<typename U>
    std::any wrap_value_(U&& value2wrap) const;

    void hash_(hasher_reference h) const override;
};

// -- Inline Implementations ---------------------------------------------------

#define TEMPLATE_PARAMS template<typename T>
#define ANY_INPUT_WRAPPER AnyInputWrapper<T>

TEMPLATE_PARAMS
template<typename U, typename>
ANY_INPUT_WRAPPER::AnyInputWrapper(U&& value2wrap) :
  base_type(wrap_value_(std::forward<U>(value2wrap))) {}

TEMPLATE_PARAMS
typename ANY_INPUT_WRAPPER::input_base_pointer ANY_INPUT_WRAPPER::clone_()
  const {
    return std::make_unique<my_type>(*this);
}

TEMPLATE_PARAMS
bool ANY_INPUT_WRAPPER::are_equal_(const AnyFieldBase& rhs) const noexcept {
    if(!base_type::are_equal_(rhs)) return false;
    return dynamic_cast<const my_type*>(&rhs) != nullptr;
}

TEMPLATE_PARAMS
bool ANY_INPUT_WRAPPER::storing_reference_() const noexcept {
    return base_type::template is_const_ref<T>;
}

TEMPLATE_PARAMS
bool ANY_INPUT_WRAPPER::storing_const_value_() const noexcept {
    return base_type::template is_const_value<T>;
}

TEMPLATE_PARAMS
void ANY_INPUT_WRAPPER::hash_(hasher_reference h) const {
    h(base_type::template cast<const std::decay_t<T>&>());
}

TEMPLATE_PARAMS
template<typename U>
std::any ANY_INPUT_WRAPPER::wrap_value_(U&& value2wrap) const {
    return std::make_any<wrapped_type>(std::forward<U>(value2wrap));
}

#undef ANY_INPUT_WRAPPER
#undef TEMPLATE_PARAMS

} // namespace pluginplay::detail_
