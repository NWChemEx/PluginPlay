#pragma once
#include "pluginplay/any/any_field.hpp"
#include <memory>

namespace pluginplay {
namespace detail_ {
class AnyResultBase;
}

/** @brief Type-erasure for results in the any hierarchy.
 *
 *  This class will be fleshed out in a later commit. For right now it's only
 *  used to set the types for the hierarchy.
 *
 */
class AnyResult : public AnyField {
private:
    using base_type = AnyField;

public:
    using result_base_pointer = std::unique_ptr<detail_::AnyResultBase>;
};

} // namespace pluginplay
