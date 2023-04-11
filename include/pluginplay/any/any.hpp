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
#include "pluginplay/any/any_field.hpp"
#include "pluginplay/any/detail_/any_field_wrapper.hpp"

namespace pluginplay::any {

/** @brief Creates an AnyField by wrapping an object of type T.
 *
 *  @tparam T The qualified (i.e. including cv-qualifiers and/or refernces) type
 *            of the object that the AnyField will wrap.
 *  @tparam Args The types of the arguments which will be forwarded to T's ctor.
 *
 *  This free function wraps the process of creating an AnyField around an
 *  object of type @p T. The actual implmentation creates an object of type
 *  @p T inplace by invoking @p T's ctor with @p args. In common usage, @p args
 *  is a @p T instance and will invoke the copy or move ctor of @p T depending
 *  on whether @p args is an lvalue or an rvalue.
 *
 *  This function will raise a static assert if @p T is a mutable reference.
 *
 *  @param[in] args The arguments to use to construct an instance of type @p T.
 *
 *  @return An AnyField instance which wraps the created @p T instance.
 *
 *  @throw ??? If the @p T ctor throws when invoked with @p args. Same throw
 *             guarantee.
 */
template<typename T, typename... Args>
auto make_any_field(Args&&... args) {
    using pimpl_type = detail_::AnyFieldWrapper<T>;
    using clean_type = std::decay_t<T>;
    static_assert(!std::is_same_v<T, clean_type&>, "Can't wrap mutable ref");
    auto pimpl = std::make_unique<pimpl_type>(T(std::forward<Args>(args)...));
    return AnyField(std::move(pimpl));
}

/** @brief Returns the object wrapped in an AnyField.
 *
 *  @tparam T The qualified type (i.e. including cv-qualifiers and references)
 *            to return.
 *  @tparam AnyType The type of the type-erased object we are unwrapping.
 *                  Expected to be a type such that std::deacy_t<AnyType> is
 *                  AnyField.
 *
 *  This free function is used to unwrap an AnyAnyField. To avoid
 *  unnecessary copies users need to fully specify the type of object to unwrap.
 *  In many cases @p T should be something like `const U&` where `U` is an
 *  unqualified type (this example would return a read-only reference to an
 *  instance of type `U`). Assuming a wrapped object of type @p U it is always
 *  possible for @p T to be:
 *
 *  - `std::decay_t<U>` (by copy)
 *  - `const std::decay_t<U>` (by read-only copy)
 *  - `const std::decay_t<U>&` (by read-only reference)
 *
 *  Additionally, for AnyFields which own non-const values, one can retrieve
 *  modifiable references, i.e. `T = std::decay_t<u>&` is also allowed.
 *
 *  Conversions which are not allowed will either trip static assertions or
 *  result in this function throwing `std::runtime_error` depending on whether
 *  or not we know at compile time if a conversion is allowed. Attempting to
 *  unwrap anything other than an AnyField will also lead to a
 *  static assertion being tripped.
 *
 *  @note It is not possible to move a mutable value out of an AnyField at this
 *        time.
 *
 *  @param[in] da_any The type-erased value we are unwrapping.
 *
 *  @return The object wrapped in @p da_any as an instance of type @p T.
 *
 *  @throw std::runtime_error if @p da_any does not currently wrap a value.
 *                            Strong throw guarantee.
 *
 *  @throw std::runtime_error if the wrapped object can not be returned as an
 *                            instance of type @p T. Strong throw guarantee.
 */
template<typename T, typename AnyType>
T any_cast(AnyType&& da_any) {
    using clean_type     = std::decay_t<AnyType>;
    constexpr bool is_ok = std::is_same_v<clean_type, AnyField>;
    static_assert(is_ok, "Object to unwrap must be an AnyField.");
    if(!da_any.has_value())
        throw std::runtime_error("Object to unwrap does not have a value.");

    return da_any.m_pimpl_->template cast<T>();
}

} // namespace pluginplay::any
