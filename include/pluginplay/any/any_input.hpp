#pragma once
#include "pluginplay/any/any_field.hpp"
#include "pluginplay/hasher.hpp"

namespace pluginplay {
namespace detail_ {
class AnyInputBase;
}

class AnyInput : public AnyField {
public:
    using hasher_type        = pluginplay::Hasher;
    using hasher_reference   = hasher_type&;
    using input_base_pointer = std::unique_ptr<detail_::AnyInputBase>;
};

} // namespace pluginplay
