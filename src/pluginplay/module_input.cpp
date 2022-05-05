#include "detail_/module_input_pimpl.hpp"
#include "pluginplay/module_input.hpp"

namespace pluginplay {

using any_check = typename ModuleInput::any_check;

ModuleInput::ModuleInput() :
  m_pimpl_(std::make_unique<detail_::ModuleInputPIMPL>()) {}

ModuleInput::ModuleInput(const ModuleInput& rhs) :
  m_is_cref_(rhs.m_is_cref_),
  m_is_actually_cref_(rhs.m_is_actually_cref_),
  m_pimpl_(rhs.m_pimpl_->clone()) {}

ModuleInput::ModuleInput(ModuleInput&& rhs) noexcept = default;

ModuleInput& ModuleInput::operator=(ModuleInput&& rhs) noexcept = default;

ModuleInput::~ModuleInput() noexcept = default;

bool ModuleInput::has_type() const noexcept { return m_pimpl_->has_type(); }

bool ModuleInput::has_value() const noexcept { return m_pimpl_->has_value(); }

bool ModuleInput::has_description() const noexcept {
    return m_pimpl_->has_description();
}

bool ModuleInput::is_optional() const noexcept {
    return m_pimpl_->is_optional();
}

bool ModuleInput::is_transparent() const noexcept {
    return m_pimpl_->is_transparent();
}

bool ModuleInput::ready() const noexcept { return m_pimpl_->is_ready(); }

const type::description& ModuleInput::description() const {
    return m_pimpl_->description();
}

ModuleInput& ModuleInput::set_description(type::description desc) noexcept {
    m_pimpl_->set_description(std::move(desc));
    return *this;
}

ModuleInput& ModuleInput::make_optional() noexcept {
    m_pimpl_->make_optional();
    return *this;
}

ModuleInput& ModuleInput::make_required() noexcept {
    m_pimpl_->make_required();
    return *this;
}

ModuleInput& ModuleInput::make_transparent() noexcept {
    m_pimpl_->make_transparent();
    return *this;
}

ModuleInput& ModuleInput::make_opaque() noexcept {
    m_pimpl_->make_opaque();
    return *this;
}

typename ModuleInput::bounds_check_desc_t ModuleInput::check_descriptions()
  const {
    return m_pimpl_->check_descriptions();
}

const type::any& ModuleInput::get_() const { return m_pimpl_->value(); }

void ModuleInput::change_(type::any new_value) {
    m_pimpl_->set_value(std::move(new_value));
}

bool ModuleInput::is_valid_(const type::any& new_value) const {
    return m_pimpl_->is_valid(new_value);
}

void ModuleInput::set_type_(const std::type_info& type) {
    m_pimpl_->set_type(type);
}

ModuleInput& ModuleInput::add_check_(any_check check, type::description desc) {
    m_pimpl_->add_check(std::move(check), std::move(desc));
    return *this;
}

bool ModuleInput::operator==(const ModuleInput& rhs) const noexcept {
    return *m_pimpl_ == *rhs.m_pimpl_;
}

} // namespace pluginplay
