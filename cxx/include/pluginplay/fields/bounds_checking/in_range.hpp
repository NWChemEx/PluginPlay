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

// -- Inline Implementations ---------------------------------------------------

template<typename T>
std::string InRange<T>::str() const {
    return "in [" + std::to_string(m_low) + ", " + std::to_string(m_high) + ")";
}

} // namespace pluginplay::bounds_checking
