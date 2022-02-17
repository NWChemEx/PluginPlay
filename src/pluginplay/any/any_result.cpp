#include "pluginplay/any/any_result.hpp"
#include "pluginplay/any/detail_/any_result_base.hpp"

namespace pluginplay {

AnyResult::AnyResult() noexcept = default;

AnyResult::AnyResult(result_base_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

AnyResult::~AnyResult() noexcept = default;

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
