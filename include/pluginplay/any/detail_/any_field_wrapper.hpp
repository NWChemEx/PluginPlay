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
#include "pluginplay/any/detail_/any_field_base.hpp"
#include "pluginplay/any/detail_/any_field_wrapper_traits.hpp"

namespace pluginplay::any::detail_ {

/** @brief Given knowledge of type @p T, implements AnyFieldBase
 *
 *  AnyFieldBase defines the type-erased API for working with the wrapped value.
 *  The implementation of most functions occurrs in this class since it has
 *  access to the object's type and can perform the operations in a type-safe
 *  manner.
 *
 *  @tparam T The type of the wrapped object. Let U = std::decay_t<T>, then T
 *            can be: U, const U, or const U&. U must be copyable, comaprable
 *            via operator== and operator<, and serializable.
 */
template<typename T>
class AnyFieldWrapper : public AnyFieldBase {
private:
    /// Type this class derives from
    using base_type = AnyFieldBase;

    /// Type of this class, inluding the template parameter
    using my_type = AnyFieldWrapper<T>;

    /// The type we are wrapping without cv-qualifiers or references
    using clean_type = std::decay_t<T>;

    /// Type of T if it it's read-only by value
    using const_value_type = const clean_type;

    /// Type of T if it's a const reference
    using const_ref_type = const clean_type&;

    /// Are we wrapping a value
    static constexpr bool wrap_val_v = std::is_same_v<clean_type, T>;

    /// Are we wrapping a const value
    static constexpr bool wrap_const_val_v =
      std::is_same_v<const_value_type, T>;

    /// Are we wrapping a const reference
    static constexpr bool wrap_const_ref_v = std::is_same_v<const_ref_type, T>;

    /// Type we're actually wrapping if we need to use a reference wrapper
    using ref_wrapper_t = std::reference_wrapper<const_value_type>;

public:
    /// Pointer to the base class of this hierarchy
    using typename base_type::field_base_pointer;

    /// Type used to wrap Python objects
    using typename base_type::python_value;

    /// This is the type of the object actually in the std::any
    using wrapped_type = std::conditional_t<wrap_const_ref_v, ref_wrapper_t, T>;

    /** @brief Makes an AnyFieldWrapper by wrapping the provided value.
     *
     *  @tparam U Type of the value we are wrapping. @p U must be implicitly
     *            convertible to @p T.
     *  @tparam <anonymous> Used to disable this function via SFINAE when @p U
     *                      is an instantiation of an input wrapper.
     *
     *  This ctor wraps the provided value in a newly created AnyFieldWrapper.
     *  Exactly how this is done depends on how @p value2wrap is passed and the
     *  exact type of @p T.
     *
     *  - When T is a value or const value the resulting AnyFieldWrapper
     *    instance will own the wrapped value. This means the wrapped value will
     *    be a copy of @p value2wrap if @p value2wrap was passed by reference
     *    and it will take ownership of @p value2wrap if it was passed by value
     *    or rvalue.
     *  - When T is a const reference AnyFieldWrapper will only alias
     *    @p value2wrap and the user is responsible for keeping @p value2wrap
     *    alive for the lifetime of the AnyFieldWrapper.
     *
     *  N.B. if @p U is not an implicityly convertible to type @p T you will get
     *  a compiler error saying there is no valid std::any ctor for @p U.
     *
     *  @param[in] value2wrap The object being passed as an input.
     *
     *  @throw ??? If wrapping @p value2wrap in a std::any throws. Same throw
     *             guarantee.
     */
    template<typename U,
             typename = disable_if_any_field_wrapper_t<std::decay_t<U>>>
    AnyFieldWrapper(U&& value2wrap);

protected:
    /// Implements polymorphic clone
    field_base_pointer clone_() const override;

    /// Implements AnyFieldBase::are_equal
    bool are_equal_(const AnyFieldBase& rhs) const noexcept override;

    /// Implements AnyFieldBase::value_equal
    bool value_equal_(const AnyFieldBase& rhs) const noexcept override;

    /** @brief Implements AnyFieldBase::print
     *
     *  This function implements AnyFieldBase::print by determining if
     *  `std::ostream::operator<<` is defined for type @p T. If it has it simply
     *  calls the stream insertion operator on the wrapped value. If it has not
     *  been defined then this function will print the name of the type and the
     *  address of the wrapped object.
     *
     *  @param[in,out] os The stream we are printing to. After this call @p os
     *                    will contain the string representation of the wrapped
     *                    object.
     *  @return @p os after adding the wrapped value to it.
     */
    std::ostream& print_(std::ostream& os) const override;

private:
    /// Implements type() for both AnyResultWrapper and AnyInputWrapper
    rtti_type type_() const noexcept override { return {typeid(T)}; }

    /// Implements as_python_wrapper()
    python_value as_python_wrapper_() const override;

    /// Implements storing_const_ref
    bool storing_const_ref_() const noexcept override;

    /// Implements storing_const_value
    bool storing_const_value_() const noexcept override;

    /// Implements storing_python_object
    bool storing_python_object_() const noexcept override;

    /// Code factorization for wrapping an object of type @p U in a std::any
    template<typename U>
    std::any wrap_value_(U&& value2wrap) const;
};

} // namespace pluginplay::any::detail_

#include "any_field_wrapper.ipp"
