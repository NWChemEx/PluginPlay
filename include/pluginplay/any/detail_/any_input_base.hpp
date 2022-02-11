#pragma once
#include "pluginplay/any/any_input.hpp"
#include "pluginplay/any/detail_/any_field_base.hpp"

namespace pluginplay::detail_ {

class AnyInputBase : public AnyFieldBase {
private:
    /// Type of the any this implements, used to get types
    using any_input_type = AnyInput;

public:
    /// Type of mutable Hasher instance
    using hasher_reference = typename any_input_type::hasher_reference;

    /** @brief Public API for hashing the wrapped value.
     *
     *  @param[in,out] h The Hasher object to use for hashing. After calling
     *                   this function the internal buffer of @p h will be
     *                   updated with a hash of the wrapped value.
     */
    void hash(hasher_reference h) const { hash_(h); }

private:
    /// Used by the derived class to implement hash
    virtual void hash_(hasher_reference h) const = 0;
};

} // namespace pluginplay::detail_
