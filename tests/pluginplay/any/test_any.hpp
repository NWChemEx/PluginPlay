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
#include <catch2/catch.hpp>
#include <string>
#include <type_traits>
#include <vector>

namespace testing {

using types2test = std::tuple<int, double, std::string, std::vector<double>>;

// Creates an instance of a given type whose value is not the default value.
template<typename T>
auto non_default_value() {
    if constexpr(std::is_same_v<int, T>) {
        return int{42};
    } else if constexpr(std::is_same_v<double, T>) {
        return double{3.14};
    } else if constexpr(std::is_same_v<std::string, T>) {
        return std::string{"Hello World"};
    } else if constexpr(std::is_same_v<std::vector<double>, T>) {
        return std::vector<double>{1.2, 2.3, 3.4};
    } else {
        static_assert(std::is_same_v<T, int>, "Register your type!!!!");
    }
}

} // namespace testing
