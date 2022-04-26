#pragma once
#include <sstream>
#include <type_traits>

namespace testing {

// Allows us to use strings of numbers as key/values regardless of what types
// the keys/values actually are
template<typename T>
T lexical_cast(const std::string& input) {
    if constexpr(std::is_same_v<T, std::string>) {
        return input;
    } else {
        std::stringstream ss;
        ss << input;
        T out;
        ss >> out;
        return out;
    }
}

} // namespace testing
