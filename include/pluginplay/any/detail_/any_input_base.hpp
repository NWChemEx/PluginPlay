#pragma once
#include "pluginplay/any/any_input.hpp"
#include "pluginplay/any/detail_/any_field_base.hpp"

namespace pluginplay::detail_ {

class AnyInputBase : public AnyFieldBase {
private:
    using any_input_type = AnyInput;

public:
    using hasher_reference = typename any_input_type::hasher_reference;

    void hash(hasher_reference h) const { hash_(h); }

private:
    virtual void hash_(hasher_reference h) const = 0;
};

} // namespace pluginplay::detail_
