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

/** @brief Namespace for functors used in bounds checking inputs to a module
 *
 *  Many of the bounds checks we provide for the user are likely to interfere
 *  name-wise with other classes. Hence we namespace protect them with the
 *  bounds_checking namespace.
 */
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

/** @brief Functor for determining if a value is in a specified range.
 *
 *  This class takes two values, call them low and high and is then usable as a
 *  functor for determining if a value is included in the range [low, high).
 *  That is the low-end is inclusive and the high-end is exclusive, following
 *  normal C++ conventions.
 *
 *  @tparam T The type of the values to compare.
 */
template<typename T>
struct InRange {
    /** @brief Creates a functor that compares to range [@p low, @p high)
     *
     *  @param[in] high The smallest value that the compared value can not have.
     *  @param[in] low The smallest value that the compared value can have.
     *                  Defaults to 0.
     *
     *  @throw none No throw guarantee.
     */
    InRange(T high, T low = 0) noexcept : m_high(high), m_low(low) {}

    /** @brief Determines if @p lhs is in the wrapped range.
     *
     * @tparam U The type of the value to compare to. Should be comparable to
     *           an object of type @p T.
     *
     * @param[in] lhs The value whose memebership in the range is in question.
     *
     * @return True if the value is in the range [low, high) and false
     *         otherwise.
     *
     * @throws ??? If the wrapped comparison throws. Same throw guarantee.
     */
    template<typename U>
    bool operator()(U&& rhs) const {
        return (m_low <= rhs) && (rhs < m_high);
    }

    /** @brief Computes a string representation of this range comparison
     *
     *  @return A string of the form "in [low, high)"
     *
     *  @throw std::bad_alloc if there is insufficient memory to make the
     *                        string. Strong throw gurantee.
     */
    std::string str() const;

    /// The smallest value an input can't be
    T m_high;

    /// The smallest value an input can be
    T m_low;
}; /// class InRange

/** @brief Contains template meta-programming classes for printing bounds checks
 *
 *  In order to print a bounds check we need to know if it is one of the ones
 *  defined by the pluginplay. The pluginplay::bounds_check::detail_ namespace
 * includes the machinery required for these checks.
 */
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

template<typename T>
std::string InRange<T>::str() const {
    return "in [" + std::to_string(m_low) + ", " + std::to_string(m_high) + ")";
}

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
