#include "pluginplay/any/any_field.hpp"
#include "pluginplay/any/detail_/any_field_base.hpp"

namespace pluginplay::any {

// -----------------------------------------------------------------------------
// -- CTors and Assignment
// -----------------------------------------------------------------------------

AnyField::AnyField(pimpl_pointer pimpl) noexcept : m_pimpl_(std::move(pimpl)) {}

AnyField::AnyField(const AnyField& other) :
  m_pimpl_(other.has_value() ? other.m_pimpl_->clone() : nullptr) {}

AnyField::AnyField(AnyField&& other) noexcept = default;

AnyField& AnyField::operator=(const AnyField& rhs) {
    if(this != &rhs) AnyField(rhs).swap(*this);
    return *this;
}

AnyField& AnyField::operator=(AnyField&& rhs) noexcept = default;

AnyField::~AnyField() noexcept = default;

void AnyField::swap(AnyField& other) noexcept { m_pimpl_.swap(other.m_pimpl_); }

void AnyField::reset() noexcept { m_pimpl_.reset(); }

typename AnyField::rtti_type AnyField::type() const noexcept {
    if(!has_value()) return rtti_type{typeid(nullptr)};
    return m_pimpl_->type();
}

bool AnyField::operator==(const AnyField& rhs) const noexcept {
    // xor condition, checks if one has PIMPL and the other doesn't
    if(has_value() != rhs.has_value()) return false;

    // Either we both do or both don't have PIMPLs, check for latter
    if(!has_value()) return true;

    // Both have PIMPLs compare them
    return m_pimpl_->value_equal(*rhs.m_pimpl_);
}

bool AnyField::are_equal(const AnyField& rhs) const noexcept {
    // xor condition, checks if one has PIMPL and the other doesn't
    if(has_value() != rhs.has_value()) return false;

    // Either we both do or both don't have PIMPLs, check for latter
    if(!has_value()) return true;

    // Both have PIMPLs compare them
    return m_pimpl_->are_equal(*rhs.m_pimpl_);
}

// bool AnyField::operator<(const AnyField& rhs) const noexcept {
//     // If RHS doesn't have a value we can't be first
//     if(!rhs.has_value()) return false;

//     // RHS has a value, if we don't then we're first
//     if(!has_value()) return true;

//     // Getting here means they both have a value, compare them
//     return m_pimpl_->value_less(*rhs.m_pimpl_);
// }

std::ostream& AnyField::print(std::ostream& os) const {
    if(!has_value()) return os;
    return m_pimpl_->print(os);
}

bool AnyField::has_value() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

bool AnyField::owns_value() const noexcept {
    if(!has_value()) return false;

    // Only way we can alias it is by const reference
    return !m_pimpl_->storing_const_reference();
}

} // namespace pluginplay::any
