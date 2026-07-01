/*
 * Copyright 2023 NWChemEx-Project
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
#include <pluginplay/types.hpp>
#include <string>
#include <type_traits>
#include <utilities/printing/demangler.hpp>

namespace pluginplay::bounds_checking {

/** @brief Wraps checking if an object is of type @p T.
 *
 *  This functor is meant for use with ModuleInput/ModuleResult where it's
 *  primary use is in being able to unwrap a pluginplay::any::AnyField. When
 *  used with objects that are not type-erased it is a functor version of
 *  std::is_convertible_v.
 *
 *  @tparam T The type that the value must be implicitly convertible to. Can be
 *            either an unqualified type or a cv-qualified reference.
 */
template<typename T>
class TypeCheck {
public:
    /** @brief Determines if @p value is of or can be converted to type @p T.
     *
     *  If @p value is an AnyField object this method will call the
     *  is_convertible method of @p value. Otherwise this method is a thin
     *  wrapper around std::is_convertible.
     *
     *  @tparam U The type of @p value. Automatically deduced by the compiler.
     *
     *  @param[in] value The object whose type will be compared to @p T.
     *
     *  @return True if @p value is convertible to type @p T and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename U>
    bool operator()(U&& value) const {
        using clean_type      = std::decay_t<U>;
        constexpr bool is_any = std::is_same_v<clean_type, type::any>;
        if constexpr(is_any) {
            return value.template is_convertible<T>() ||
                   value.template is_convertible<const T&>();
        } else {
            return std::is_convertible_v<U, T>;
        }
    }

    /** @brief Determines a string representation of the type check.
     *
     *  This method generates a string which indicates the type *this is
     *  looking for.
     *
     *  @return A string representation of *this.
     */
    std::string str() const {
        const std::string p0("Type == ");
        const auto p1 = utilities::printing::Demangler::demangle(typeid(T));
        return p0 + p1;
    }
};

} // namespace pluginplay::bounds_checking
