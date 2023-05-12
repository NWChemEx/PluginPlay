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

/** @brief Implements a functor capable of being used as a bounds check with the
 *         pluginplay.
 *
 *  This class wraps a binary functor and the value to compare to. In practice
 *  this binds the wrapped value to the second argument of the binary op. While
 *  binding the argument to the second argument may seem odd at first, consider
 *  an example where we create an instance `fxn`:
 *
 *  @code
 *  CompareTo<double, std::greater<double>> fxn(3.14);
 *
 *  //Is 2.0 > 3.14
 *  assert(!fxn(2.0));
 *
 *  //Is 4.1 > 3.14;
 *  assert(fxn(4.1)):
 *  @endcode
 *
 *  As can be seen it's the nature of this class that we are binding the right
 *  argument to the functor.
 *
 * @tparam T The type of the values being compared
 * @tparam Op The functor to actually do the comparison
 */
template<typename T, typename Op>
struct CompareTo {
    /** @brief Creates a new comparison with the right side bound to @p value
     *
     *  The resulting instance is a functor, which can be used to compare values
     *  to the the bound value using operator().
     *
     *  @param[in] value The value to bind to the right side of the operation.
     *
     *  @throw none No throw guarantee.
     */
    CompareTo(T value) noexcept : m_value(value) {}

    /** @brief Determines if @p lhs passes the wrapped conditional check
     *
     * @tparam U The type of the value to compare to. Should be comparable to
     *           an object of type @p T.
     *
     * @param[in] lhs The value to apply the wrapped comparison to.
     *
     * @return True if the internal comparison returns true and false otherwise.
     *
     * @throws ??? If the wrapped comparison throws. Same throw guarantee.
     */
    template<typename U>
    bool operator()(U&& lhs) const;

    /** @brief Returns a string representation of this check.
     *
     *  As an example, if this check ensures that a value is greater than three,
     *  this function will ultimately print out something like "> 3".
     *
     *  @return A string representation of the comparison.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        string. Strong throw guarantee.
     */
    std::string str() const;

    /// The actual op to apply
    Op m_op;

    /// The value bound to the right of the op
    T m_value;
}; // class CompareTo

/// CompareTo with the comparison op set to !=
template<typename T>
using NotEqualTo = CompareTo<T, std::not_equal_to<T>>;

/// CompareTo with the comparison op set to >
template<typename T>
using GreaterThan = CompareTo<T, std::greater<T>>;

/// CompareTo with the comparison op set to <
template<typename T>
using LessThan = CompareTo<T, std::less<T>>;

/// CompareTo with the comparison op set to >=
template<typename T>
using GreaterThanEqualTo = CompareTo<T, std::greater_equal<T>>;

/// CompareTo with the comparison op set to <=
template<typename T>
using LessThanEqualTo = CompareTo<T, std::less_equal<T>>;

//-----------------------Implementations----------------------------------------

template<typename T, typename Op>
template<typename U>
bool CompareTo<T, Op>::operator()(U&& lhs) const {
    return m_op(std::forward<U>(lhs), m_value);
}

template<typename T, typename Op>
std::string CompareTo<T, Op>::str() const {
    std::string rv;

    constexpr bool is_not_equal     = std::is_same_v<Op, std::not_equal_to<T>>;
    constexpr bool is_greater       = std::is_same_v<Op, std::greater<T>>;
    constexpr bool is_less          = std::is_same_v<Op, std::less<T>>;
    constexpr bool is_greater_equal = std::is_same_v<Op, std::greater_equal<T>>;
    constexpr bool is_less_equal    = std::is_same_v<Op, std::less_equal<T>>;

    if constexpr(is_not_equal) {
        rv += "!=";
    } else if constexpr(is_greater) {
        rv += ">";
    } else if constexpr(is_less) {
        rv += "<";
    } else if constexpr(is_greater_equal) {
        rv += ">=";
    } else if constexpr(is_less_equal) {
        rv += "<=";
    } else {
        rv += "<undefined operator> ";
    }

    return rv + " " + std::to_string(m_value);
}

} // namespace pluginplay::bounds_checking
