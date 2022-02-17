#pragma once
#include <typeindex>

namespace pluginplay {
namespace detail_ {
class AnyFieldBase;
}

/** @brief Base class of Any hierarchy.
 *
 *  This class will be fleshed out in a later commit. For right now it's only
 *  used to set the types for the hierarchy.
 *
 */

class AnyField {
public:
    using rtti_type = std::type_index;
};

} // namespace pluginplay
