#include "SDE/ModuleResult.hpp"
#include <typeindex>

namespace SDE {

using shared_any = typename ModuleResult::shared_any;
using pimpl_t    = detail_::ModuleResultPIMPL;

namespace detail_ {

struct ModuleResultPIMPL {
    shared_any value;
    type::description desc = "";
    std::type_index type   = std::type_index(typeid(std::nullptr_t));
};

bool operator==(const pimpl_t& lhs, const pimpl_t& rhs) {
    return std::tie(*lhs.value, lhs.desc, lhs.type) ==
           std::tie(*rhs.value, rhs.desc, rhs.type);
}

} // namespace detail_

using MO = ModuleResult;

MO::ModuleResult() : pimpl_(std::make_unique<pimpl_t>()) {}
MO::ModuleResult(const MO& rhs) :
  pimpl_(std::make_unique<pimpl_t>(*rhs.pimpl_)) {}
MO& MO::operator=(const MO& rhs) {
    std::make_unique<pimpl_t>(*rhs.pimpl_).swap(pimpl_);
    return *this;
}
MO::ModuleResult(MO&& rhs) noexcept = default;
MO& MO::operator=(MO&& rhs) noexcept = default;
MO::~ModuleResult() noexcept         = default;

const type::description& MO::description() const noexcept {
    return pimpl_->desc;
}

MO& MO::set_type_(const std::type_info& rtti) noexcept {
    pimpl_->type = std::type_index(rtti);
    return *this;
}

const shared_any& MO::at_() const noexcept { return pimpl_->value; }

void MO::change_(type::any new_value) {
    if(pimpl_->type == std::type_index(typeid(std::nullptr_t)))
        throw std::runtime_error("Call set_type with a valid type first.");
    if(std::type_index(new_value.type()) != pimpl_->type)
        throw std::invalid_argument("New value is incorrect type");
    change_(std::make_shared<type::any>(std::move(new_value)));
}

void MO::change_(shared_any new_value) noexcept { pimpl_->value = new_value; }

MO& MO::set_description(type::description desc) noexcept {
    pimpl_->desc = std::move(desc);
    return *this;
}

bool MO::operator==(const MO& rhs) const { return *pimpl_ == *rhs.pimpl_; }

} // namespace SDE
