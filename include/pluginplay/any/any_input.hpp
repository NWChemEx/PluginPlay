#pragma once
#include "pluginplay/any/any_field.hpp"
#include "pluginplay/hasher.hpp"

namespace pluginplay {

class AnyInput : public AnyField {
public:
    using hasher_type      = pluginplay::Hasher;
    using hasher_reference = hasher_type&;
};

} // namespace pluginplay
