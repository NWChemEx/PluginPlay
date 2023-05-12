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
#include <functional> //For comparison functors
#include <string>

namespace pluginplay::bounds_checking {

template<typename T, typename Op>
class CompareTo;

template<typename T>
class InRange;

namespace detail_ {

/** @brief Primary template for printing a bounds check, just prints the empty
 *         string.
 *
 *  In order to print a bounds check functor one specializes this class for that
 *  functor.
 *
 * @tparam T The type of the functor to print out.
 */
template<typename T>
struct PrintableBoundsCheck {
    /** @brief Member that returns the string representation of the functor
     *
     *  This function just returns the empty string. Specializations of this
     *  class will in general return actual strings. This function will fail to
     *  compile if @p U is not implicitly convertible to type @p T.
     *
     *  @tparam U The type of the particular functor we are printing out.
     *            Should be implicitly convertible to @p T.
     *
     *  @param[in] <unnamed> The functor to print out.
     *
     *  @return The empty string
     *
     *  @throw none No throw guarantee.
     */
    template<typename U>
    static std::string str(U&&) noexcept;
};

/// Specializes PrintableBoundsCheck to ComapreTo checks
template<typename T, typename Op>
struct PrintableBoundsCheck<CompareTo<T, Op>> {
    /** @brief Calls the `str` member of the provided CompareTo functor.
     *
     *  This function will fail to compile if @p U is not implicitly convertible
     *  to @p T.
     *
     *  @tparam U The type of the functor we are printing out. Must be
     *            implicitly convertible to
     *
     * @param[in] check The particular functor instance to print.
     *
     * @return The CompareTo functor as a string.
     *
     * @throw std::bad_alloc if the CompareTo functor's str() member has
     *                       insufficient memory to make the string. Strong
     *                       throw guarantee.
     */
    template<typename U>
    static std::string str(U&& check);
};

/// Specializes PrintableBoundsCheck to InRange checks
template<typename T>
struct PrintableBoundsCheck<InRange<T>> {
    /** @brief Calls the `str` member of the provided InRange functor.
     *
     *  This function will fail to compile if @p U is not implicitly convertible
     *  to @p T.
     *
     * @tparam U The type of the functor we are printing out. Must be
     *           implicitly convertible to
     *
     * @param[in] check The particular functor instance to print.
     *
     * @return The InRange functor as a string.
     *
     * @throw std::bad_alloc if the InRange functor's str() member has
     *                       insufficient memory to make the string. Strong
     *                       throw guarantee.
     */
    template<typename U>
    static std::string str(U&& check);
};

} // namespace detail_

/** @brief Used to pretty print a bounds check functor.
 *
 *  If a bounds check functor is one of the pluginplay's built in bounds checks
 * we can just call its `str` method to get a string representation of the
 * check. To determine if we can call `str` we rely on the helper class
 *  PrintableBoundsCheck and its specialization. This function is a convenience
 *  wrapper around the call to PrintableBoundsCheck.
 *
 *  @tparam T The type of the bounds
 *
 *  @param[in] check The bounds check that we want a string representation of.
 *                   @p check will not be modified by this function.
 *
 *  @return The empty string if @p check is not a specialization of CompareTo
 *          and a string representation of @p check otherwise.
 *
 *  @throw std::bad_alloc if there is insufficient memory to create the string
 *                        representation. Strong throw guarantee.
 */
template<typename T>
std::string print_bounds(T&& check);

// -- Inline Implementations ---------------------------------------------------

namespace detail_ {

template<typename T>
template<typename U>
std::string PrintableBoundsCheck<T>::str(U&& check) noexcept {
    using clean_t = std::decay_t<U>;
    static_assert(std::is_same_v<clean_t, T>, "Must be type T");
    return "";
}

template<typename T, typename Op>
template<typename U>
std::string PrintableBoundsCheck<CompareTo<T, Op>>::str(U&& check) {
    using clean_t = std::decay_t<U>;
    static_assert(std::is_same_v<clean_t, CompareTo<T, Op>>, "Must be type T");
    return check.str();
}

template<typename T>
template<typename U>
std::string PrintableBoundsCheck<InRange<T>>::str(U&& check) {
    using clean_t = std::decay_t<U>;
    static_assert(std::is_same_v<clean_t, InRange<T>>, "Must be type T");
    return check.str();
}

} // namespace detail_

template<typename T>
std::string print_bounds(T&& check) {
    using bounds_check = detail_::PrintableBoundsCheck<std::decay_t<T>>;
    return bounds_check::str(std::forward<T>(check));
}

} // namespace pluginplay::bounds_checking
