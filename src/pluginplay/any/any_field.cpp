#include "pluginplay/any/any_field.hpp"

#include "pluginplay/any/detail_/any_field_base.hpp"

namespace pluginplay {

typename AnyField::rtti_type AnyField::type() const noexcept {
    if(!has_value()) return rtti_type{typeid(nullptr)};
    return base_pimpl_().type();
}

bool AnyField::are_equal(const AnyField& rhs) const noexcept {
    // Take care of xor condition (which means they're not equal)
    if(has_value() != rhs.has_value()) return false;
    if(!has_value()) return true; // Both of them must not have a value

    // Getting here means they both have a value, compare it
    return base_pimpl_().are_equal(rhs.base_pimpl_());
}

std::ostream& AnyField::print(std::ostream& os) const {
    if(!has_value()) return os;
    return base_pimpl_().print(os);
}

} // namespace pluginplay
