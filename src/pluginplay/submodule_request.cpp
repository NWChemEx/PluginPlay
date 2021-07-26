#include "detail_/submodule_request_pimpl.hpp"

namespace pluginplay {

using module_ptr = typename SubmoduleRequest::module_ptr;

SubmoduleRequest::SubmoduleRequest() :
  m_pimpl_(std::make_unique<detail_::SubmoduleRequestPIMPL>()) {}

SubmoduleRequest::SubmoduleRequest(const SubmoduleRequest& rhs) :
  m_pimpl_(rhs.m_pimpl_->clone()) {}

SubmoduleRequest::SubmoduleRequest(SubmoduleRequest&& rhs) noexcept = default;

SubmoduleRequest& SubmoduleRequest::operator=(SubmoduleRequest&&) noexcept =
  default;

SubmoduleRequest::~SubmoduleRequest() noexcept = default;

bool SubmoduleRequest::has_type() const noexcept {
    return m_pimpl_->has_type();
}

bool SubmoduleRequest::has_module() const noexcept {
    return m_pimpl_->has_module();
}

bool SubmoduleRequest::has_description() const noexcept {
    return m_pimpl_->has_description();
}

bool SubmoduleRequest::ready() const noexcept { return m_pimpl_->ready(); }

void SubmoduleRequest::change(module_ptr new_mod) {
    m_pimpl_->set_module(new_mod);
}

SubmoduleRequest& SubmoduleRequest::set_description(
  type::description desc) noexcept {
    m_pimpl_->set_description(std::move(desc));
    return *this;
}

type::rtti SubmoduleRequest::type() const { return m_pimpl_->type(); }

const Module& SubmoduleRequest::value() const { return m_pimpl_->value(); }

Module& SubmoduleRequest::value() { return m_pimpl_->value(); }

const type::description& SubmoduleRequest::description() const {
    return m_pimpl_->description();
}

void SubmoduleRequest::lock() { m_pimpl_->lock(); }

void SubmoduleRequest::hash(Hasher& h) const {
    if(m_pimpl_->has_module()) value().hash(h);
}

bool SubmoduleRequest::operator==(const SubmoduleRequest& rhs) const {
    return *m_pimpl_ == *rhs.m_pimpl_;
}

void SubmoduleRequest::set_type_(const std::type_info& type,
                                 type::input_map inputs) {
    m_pimpl_->set_type(std::type_index(type), std::move(inputs));
}

} // namespace pluginplay
