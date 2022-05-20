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
