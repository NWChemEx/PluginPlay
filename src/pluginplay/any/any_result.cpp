#include "pluginplay/any/any_result.hpp"
#include "pluginplay/any/detail_/any_result_base.hpp"

namespace pluginplay {

AnyResult::AnyResult() noexcept = default;

AnyResult::AnyResult(result_base_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

AnyResult::AnyResult(const AnyResult& other) :
  m_pimpl_(other.has_value() ? other.m_pimpl_->clone() : nullptr) {}

AnyResult::AnyResult(AnyResult&& other) noexcept :
  m_pimpl_(std::move(other.m_pimpl_)) {}

AnyResult& AnyResult::operator=(const AnyResult& rhs) {
    if(this == &rhs) return *this;
    AnyResult(rhs).swap(*this);
    return *this;
}

AnyResult& AnyResult::operator=(AnyResult&& rhs) noexcept {
    if(this == &rhs) return *this;
    m_pimpl_ = std::move(rhs.m_pimpl_);
    return *this;
}

AnyResult::~AnyResult() noexcept = default;

// -- Protected Methods --------------------------------------------------------

bool AnyResult::are_equal_(const base_type& rhs) const noexcept {
    auto p = dynamic_cast<const AnyResult*>(&rhs);
    return p == nullptr ? false : base_type::are_equal_(rhs);
}

void AnyResult::reset_() noexcept { m_pimpl_.reset(); }

bool AnyResult::has_value_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

typename AnyResult::field_base_reference AnyResult::base_pimpl_() {
    assert_value_();
    return *m_pimpl_;
}

typename AnyResult::const_field_base_reference AnyResult::base_pimpl_() const {
    assert_value_();
    return *m_pimpl_;
}

typename AnyResult::result_base_reference AnyResult::pimpl_() {
    assert_value_();
    return *m_pimpl_;
}

typename AnyResult::const_result_base_reference AnyResult::pimpl_() const {
    assert_value_();
    return *m_pimpl_;
}

} // namespace pluginplay
