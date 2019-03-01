#include "SDE/SubmoduleRequest.hpp"
#include "SDE/detail_/SubmoduleRequestPIMPL.hpp"

namespace SDE {
using module_ptr = typename SubmoduleRequest::module_ptr;

SubmoduleRequest::SubmoduleRequest() :
  pimpl_(std::make_unique<detail_::SubmoduleRequestPIMPL>()) {}
SubmoduleRequest::SubmoduleRequest(const SubmoduleRequest& rhs) :
  pimpl_(rhs.pimpl_->clone()) {}
SubmoduleRequest::SubmoduleRequest(SubmoduleRequest&& rhs) noexcept = default;
SubmoduleRequest& SubmoduleRequest::operator=(SubmoduleRequest&&) noexcept =
  default;

SubmoduleRequest::~SubmoduleRequest() noexcept = default;

const Module& SubmoduleRequest::value() const { return pimpl_->value(); }

const type::description& SubmoduleRequest::description() const noexcept {
    return pimpl_->m_desc;
}

void SubmoduleRequest::change(module_ptr new_mod) noexcept {
    pimpl_->m_module = new_mod;
}

SubmoduleRequest& SubmoduleRequest::set_description(
  type::description desc) noexcept {
    pimpl_->m_desc = std::move(desc);
    return *this;
}

void SubmoduleRequest::hash(type::hasher& h) const {
    if(pimpl_->m_module) value().hash(h);
}

bool SubmoduleRequest::ready() const noexcept { pimpl_->ready(); }

void SubmoduleRequest::lock() { pimpl_->lock(); }

const std::type_index& SubmoduleRequest::type() const { return pimpl_->m_type; }

bool SubmoduleRequest::check_type_(const std::type_info& type) const noexcept {
    return pimpl_->check_type(type);
}

void SubmoduleRequest::set_type_(const std::type_info& type,
                                 type::input_map inputs) {
    pimpl_->set_type(type, std::move(inputs));
}

bool SubmoduleRequest::operator==(const SubmoduleRequest& rhs) const {
    return *pimpl_ == *rhs.pimpl_;
}

} // namespace SDE
