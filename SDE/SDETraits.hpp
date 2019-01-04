#pragma once
#include <string>
#include <typeindex>

namespace SDE {
namespace detail_ {

struct SDETraits {
    using description_type = std::string;
    using key_type         = std::string;
    using rtti_type        = std::type_index;
};

} // namespace detail_
} // namespace SDE
