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
#include <iostream>
#include <memory>
#include <type_traits>

/** @file Utility.hpp
 *
 *  This file contains odds and ends needed by the pluginplay that are too small
 * to warrant their own header/source files and do not reasonably fit in
 *  elsewhere.
 *
 *  Note that examples of the meta-templating structs can be found in the
 *  tests (of course in practice one will be passing template type parameters to
 *  the structs and not hard-coded types).
 */

namespace pluginplay::detail_ {

/** @brief Determines if a type is a C-string.
 *
 *  It's natural for users of the pluginplay to input strings as string
 * literals, *i.e.*, things like `"a string literal"`. Particularly from the
 *  perspective of template meta-programming, string literals are not the
 *  same thing as `std::string`. This struct allows us to determine at compile
 *  time whether a particular type is a string literal (which we call a C string
 *  for short/because they are strings in the C sense).
 *
 *  This struct works by inheriting from `std::false_type` (which gives it a
 *  `static` `constexpr` member named `value` whose value is false). Then we
 *  specialize the struct so that if `T=const char (&)[N]`, the type of a
 *  string literal, it instead inherits from `std::true_type` (which is the
 *  same as `std::false_type` except the value of `value` is true). We can not
 *  use `std::is_same` because we can't list all the types that `T` is allowed
 *  to equal and still be a C-string since there is one type per value of `N`.
 *
 *  @tparam T The type we want to know whether or not it is a C-string
 */
template<typename T>
struct IsCString : std::false_type {};

/** @brief Specilization of `IsCString` for case when the type is a string
 * literal.
 *
 *  See the documentation of the primary template for more details.
 *
 * @tparam N The number of characters in the string literal, including the
 *         termination character, `\0`.
 */
template<std::size_t N>
struct IsCString<const char (&)[N]> : std::true_type {};

/** @brief Struct for determining whether or not a particular type is a const
 *         reference.
 *
 *  When forwarding arguments in the pluginplay we often need to take special
 * care if the user has told us that a particular input should be taken by const
 *  reference. This typedef is meant to serve as a more readable type for the
 *  struct which contains a member `value` whose value is true if template
 *  parameter is a const reference and false otherwise.
 *
 *  @param T The type we want to know the const-reference-ness of.
 */
template<typename T>
using IsConstRef = std::conjunction<std::is_reference<T>,
                                    std::is_const<std::remove_reference_t<T>>>;

/** @brief Struct for determining whether or not a particular type is a
 *         shared_ptr.
 *
 *  When dealing with the results they are always stored in `shared_ptr`
 *  instances because they often will exist in the cache as well as be in use
 *  throughout the code. This struct allows us to determine at compile-time
 *  whether a particular input is a `shared_ptr` or not.
 *
 *  This struct works by inheriting from `std::false_type` (which gives it a
 *  `static` `constexpr` member named `value` whose value is false). Then we
 *  specialize the struct so that if `T=std::shared_ptr<U>`, it instead
 *  inherits from `std::true_type` (which is the same as `std::false_type`
 *  except the value of `value` is true). We can not use `std::is_same` because
 *  there are infinite number of possible types that are `shared_ptr`s (one for
 *  each type the `shared_ptr` can wrap).
 *
 *  @param T The type whose shared_ptr-ness is in question.
 */
template<typename T>
struct IsSharedPtr : std::false_type {};

/** @brief Specializes IsSharedPtr for when the template parameter is a
 *         `shared_ptr`.
 *
 * See the description of the IsSharedPtr primary template for more information.
 *
 * @tparam T The type of the object wrapped by the `shared_ptr`.
 */
template<typename T>
struct IsSharedPtr<std::shared_ptr<T>> : std::true_type {};

/** @brief Overloads equality operator for reference wrappers.
 *
 * This function works by calling `operator==` for the wrapped instances. Hence
 * a definition of `operator==(const LHS_t&, const RHS_t&)` needs to be
 * available. This function compares the state of the values in the reference
 * wrappers and not whether the wrappers wrap the same instance.
 *
 * @tparam LHS_t The type of the object on the left
 * @tparam RHS_t The type of the object on the right
 *
 * @param lhs The reference wrapper on the left of ==
 * @param rhs The reference wrapper on the right of ==
 *
 * @return True if the wrapped instances compare equal and false otherwise.
 *
 * @throw ??? Throws if the called `operator==` throws. Strong throw guarantee.
 */
template<typename LHS_t, typename RHS_t>
bool operator==(const std::reference_wrapper<LHS_t> lhs,
                const std::reference_wrapper<RHS_t> rhs) {
    return lhs.get() == rhs.get();
}

} // namespace pluginplay::detail_
