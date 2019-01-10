#include "SDE/SubmoduleRequest.hpp"
#include <typeindex>

namespace SDE {
using SR               = SubmoduleRequest;
using description_type = typename SR::description_type;
using module_type      = typename SR::module_type;
using module_ptr       = typename SR::module_ptr;

namespace detail_ {

struct SubmoduleRequestPIMPL {
    description_type desc;
    std::type_index type = std::type_index(typeid(std::nullptr_t));
    module_ptr module;
};

} // namespace detail_

using pimpl_t = detail_::SubmoduleRequestPIMPL;

SR::SubmoduleRequest() : pimpl_(std::make_unique<pimpl_t>()) {}
SR::SubmoduleRequest(const SR& rhs) :
  pimpl_(std::make_unique<pimpl_t>(*rhs.pimpl_)) {}
SR& SR::operator=(const SR& rhs) {
    std::make_unique<pimpl_t>(*rhs.pimpl_).swap(pimpl_);
    return *this;
}
SR::SubmoduleRequest(SR&& rhs) noexcept = default;
SR& SR::operator=(SR&& rhs) noexcept = default;
SR::~SubmoduleRequest() noexcept     = default;

const module_type& SR::value() const { return *pimpl_->module; }
const description_type& SR::description() const noexcept {
    return pimpl_->desc;
}
void SR::change(module_ptr new_mod) { pimpl_->module = new_mod; }
SR& SR::set_description(description_type desc) noexcept {
    pimpl_->desc = std::move(desc);
    return *this;
}

bool SR::check_type_(const std::type_info& type) const noexcept {
    return pimpl_->type == std::type_index(type);
}

void SR::set_type_(const std::type_info& type) noexcept {
    pimpl_->type = std::type_index(type);
}

} // namespace SDE
