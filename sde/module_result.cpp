#include "sde/detail_/module_result_pimpl.hpp"
#include "sde/module_result.hpp"
#include <typeindex>

namespace sde {

using shared_any = typename ModuleResult::shared_any;
using pimpl_t    = detail_::ModuleResultPIMPL;

ModuleResult::ModuleResult() : pimpl_(std::make_unique<pimpl_t>()) {}
ModuleResult::ModuleResult(const ModuleResult& rhs) :
  pimpl_(rhs.pimpl_->clone()) {}
ModuleResult::ModuleResult(ModuleResult&& rhs) noexcept = default;
ModuleResult& ModuleResult::operator=(ModuleResult&& rhs) noexcept = default;
ModuleResult::~ModuleResult() noexcept                             = default;

const type::description& ModuleResult::description() const noexcept {
    return pimpl_->m_desc;
}

ModuleResult& ModuleResult::set_type_(const std::type_info& rtti) {
    pimpl_->set_type(rtti);
    return *this;
}

const shared_any& ModuleResult::at_() const noexcept { return pimpl_->m_value; }

void ModuleResult::change_(type::any new_value) {
    pimpl_->change(std::make_shared<type::any>(std::move(new_value)));
}

void ModuleResult::change_(shared_any new_value) noexcept {
    pimpl_->change(new_value);
}

ModuleResult& ModuleResult::set_description(type::description desc) noexcept {
    pimpl_->m_desc = std::move(desc);
    return *this;
}

bool ModuleResult::operator==(const ModuleResult& rhs) const {
    return *pimpl_ == *rhs.pimpl_;
}

} // namespace sde
