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

namespace pluginplay {
namespace python {
class PythonWrapper;
}
namespace detail_ {

/** @brief Struct that provides clearer error message when types are not
 * convertible.
 *
 * This allows the replacement of a static assert of the form:
 * `static_assert(std::is_convertible_v<T1,T2>)`
 * with instantiation of a templated struct as:
 * `STATIC_ASSERT_CONVERTIBLE_VERBOSE<T1, T2, I>()`
 * where `I` is intended to be used as an index describing the position of the
 * argument that should have type `T2`
 *
 * The only advantage of using this over the simple `static_assert` is that it
 * will give a slightly simpler error message showing a smaller set of template
 * parameters.
 *
 * @note A special exception is made when @p T1 is an AnyField object. In this
 *       case we don't know if the AnyField object can be converted to an object
 *       of type @p T2 until runtime.
 *
 * @tparam T1 The type of the object to convert from
 * @tparam T2 The type of the object to convert to
 * @tparam I position of the argument in some argument list
 */
template<typename T1, typename T2, std::size_t I>
struct STATIC_ASSERT_CONVERTIBLE_VERBOSE {
    using clean_t1               = std::decay_t<T1>;
    static constexpr bool is_any = std::is_same_v<clean_t1, any::AnyField>;
    static_assert(std::is_convertible_v<T1, T2> || is_any,
                  R"(

    Incorrect argument type; type T1 not convertible to type T2 at position I.
    For values of T1, T2, and I, see compiler output formatted as:
        struct pluginplay::detail_::STATIC_ASSERT_CONVERTIBLE_VERBOSE<T1, T2, I>
        )");
};

} // namespace detail_
} // namespace pluginplay
