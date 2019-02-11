#include "SDE/SubmoduleRequest.hpp"
#include <typeindex>

namespace SDE {
using SR         = SubmoduleRequest;
using module_ptr = typename SR::module_ptr;
using pimpl_t    = detail_::SubmoduleRequestPIMPL;
namespace detail_ {

struct SubmoduleRequestPIMPL {
    SubmoduleRequestPIMPL() = default;
    SubmoduleRequestPIMPL(const pimpl_t& rhs) :
      desc(rhs.desc),
      type(rhs.type),
      module(std::make_shared<Module>(*rhs.module)) {}
    pimpl_t& operator=(const pimpl_t& rhs) {
        return *this = std::move(pimpl_t(rhs));
    }
    SubmoduleRequestPIMPL(pimpl_t&&) = default;
    pimpl_t& operator=(pimpl_t&&) = default;

    type::description desc;
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

const Module& SR::value() const { return *pimpl_->module; }
const type::description& SR::description() const noexcept {
    return pimpl_->desc;
}
void SR::change(module_ptr new_mod) noexcept { pimpl_->module = new_mod; }
SR& SR::set_description(type::description desc) noexcept {
    pimpl_->desc = std::move(desc);
    return *this;
}

bool SR::ready() const noexcept {
    if(!pimpl_->module) return false;
    return pimpl_->module->ready();
}

void SR::lock() {
    if(!ready()) throw std::runtime_error("Can't lock non-ready submodule");
    pimpl_->module->lock();
}

bool SR::check_type_(const std::type_info& type) const noexcept {
    return pimpl_->type == std::type_index(type);
}

void SR::set_type_(const std::type_info& type) noexcept {
    pimpl_->type = std::type_index(type);
}

} // namespace SDE
