#include "pluginplay/any/any_input.hpp"
#include "pluginplay/any/detail_/any_input_base.hpp"

namespace pluginplay {

// -- CTors and Assignment -----------------------------------------------------

AnyInput::AnyInput() noexcept = default;

AnyInput::AnyInput(input_base_pointer value) noexcept :
  m_pimpl_(std::move(value)) {}

AnyInput::AnyInput(const AnyInput& other) :
  m_pimpl_(other.has_value() ? other.m_pimpl_->clone() : nullptr) {}

AnyInput::AnyInput(AnyInput&& other) noexcept :
  m_pimpl_(std::move(other.m_pimpl_)) {}

AnyInput& AnyInput::operator=(const AnyInput& rhs) {
    if(this == &rhs) return *this;
    AnyInput(rhs).swap(*this);
    return *this;
}

AnyInput& AnyInput::operator=(AnyInput&& rhs) noexcept {
    if(this == &rhs) return *this;
    m_pimpl_ = std::move(rhs.m_pimpl_);
    return *this;
}

AnyInput::~AnyInput() noexcept = default;

// -- Public methods -----------------------------------------------------------

void AnyInput::swap(AnyInput& other) noexcept { m_pimpl_.swap(other.m_pimpl_); }

void AnyInput::hash(hasher_reference h) const {
    if(has_value()) m_pimpl_->do_hash(h);
}

// -- Protected methods --------------------------------------------------------

bool AnyInput::are_equal_(const base_type& rhs) const noexcept {
    auto p = dynamic_cast<const AnyInput*>(&rhs);
    return p == nullptr ? false : base_type::are_equal_(rhs);
}

void AnyInput::reset_() noexcept { m_pimpl_.reset(); }

bool AnyInput::has_value_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

typename AnyInput::field_base_reference AnyInput::base_pimpl_() {
    assert_value_();
    return *m_pimpl_;
}

typename AnyInput::const_field_base_reference AnyInput::base_pimpl_() const {
    assert_value_();
    return *m_pimpl_;
}

typename AnyInput::input_base_reference AnyInput::pimpl_() {
    assert_value_();
    return *m_pimpl_;
}

typename AnyInput::const_input_base_reference AnyInput::pimpl_() const {
    assert_value_();
    return *m_pimpl_;
}

} // namespace pluginplay
