#include "SDE/ModuleInput.hpp"
#include "SDE/detail_/ModuleInputPIMPL.hpp"

namespace SDE {
using any_check = typename ModuleInput::any_check;

ModuleInput::ModuleInput() :
  pimpl_(std::make_unique<detail_::ModuleInputPIMPL>()) {}

ModuleInput::ModuleInput(const ModuleInput& rhs) :
  is_cref_(rhs.is_cref_),
  is_actually_cref_(rhs.is_actually_cref_),
  pimpl_(rhs.pimpl_->clone()) {}

ModuleInput::ModuleInput(ModuleInput&& rhs) noexcept = default;
ModuleInput& ModuleInput::operator=(ModuleInput&& rhs) noexcept = default;
ModuleInput::~ModuleInput() noexcept                            = default;

bool ModuleInput::is_optional() const noexcept { return pimpl_->m_optional; }
bool ModuleInput::is_transparent() const noexcept {
    return pimpl_->m_transparent;
}
bool ModuleInput::is_ready() const noexcept { pimpl_->is_ready(); }

const type::description& ModuleInput::description() const noexcept {
    return pimpl_->m_desc;
}

void ModuleInput::hash(Hasher& h) const { return pimpl_->hash(h); }

ModuleInput& ModuleInput::set_description(type::description desc) noexcept {
    pimpl_->m_desc = std::move(desc);
    return *this;
}

ModuleInput& ModuleInput::make_optional() noexcept {
    pimpl_->m_optional = true;
    return *this;
}

ModuleInput& ModuleInput::make_required() noexcept {
    pimpl_->m_optional = false;
    return *this;
}

ModuleInput& ModuleInput::make_transparent() noexcept {
    pimpl_->m_transparent = true;
    return *this;
}

ModuleInput& ModuleInput::make_opaque() noexcept {
    pimpl_->m_transparent = false;
    return *this;
}

const type::any& ModuleInput::get_() const { return pimpl_->m_value; }

void ModuleInput::change_(type::any new_value) {
    pimpl_->change(std::move(new_value));
}

bool ModuleInput::is_valid_(const type::any& new_value) const {
    return pimpl_->is_valid(new_value);
}

void ModuleInput::set_type_(const std::type_info& type) {
    pimpl_->set_type(type);
}

ModuleInput& ModuleInput::add_check_(any_check check, type::description desc) {
    pimpl_->add_check(std::move(check), std::move(desc));
    return *this;
}

bool ModuleInput::operator==(const ModuleInput& rhs) const {
    return *pimpl_ == *rhs.pimpl_;
}

} // namespace SDE
