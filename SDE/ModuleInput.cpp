#include "SDE/ModuleInput.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <typeindex>

namespace SDE {
using description_type = typename ModuleInput::description_type;
using any_type         = typename ModuleInput::any_type;
using any_check        = typename ModuleInput::any_check;

namespace detail_ {

struct ModuleInputPIMPL {
    any_type value;
    Utilities::CaseInsensitiveMap<any_check> checks;
    std::type_index type  = std::type_index(typeid(std::nullptr_t));
    description_type desc = "";
    bool is_optional      = false;
    bool is_transparent   = false;
};

} // namespace detail_

using MI = ModuleInput;

MI::ModuleInput() : pimpl_(std::make_unique<detail_::ModuleInputPIMPL>()) {}
MI::ModuleInput(const ModuleInput& rhs) :
  pimpl_(std::make_unique<detail_::ModuleInputPIMPL>(*rhs.pimpl_)) {}
MI& MI::operator=(const SDE::ModuleInput& rhs) {
    std::make_unique<detail_::ModuleInputPIMPL>(*rhs.pimpl_).swap(pimpl_);
    return *this;
}
MI::ModuleInput(SDE::ModuleInput&& rhs) noexcept = default;
MI& MI::operator=(SDE::ModuleInput&& rhs) noexcept = default;
MI::~ModuleInput() noexcept                        = default;

bool MI::is_optional() const noexcept { return pimpl_->is_optional; }
bool MI::is_transparent() const noexcept { return pimpl_->is_transparent; }
const description_type& MI::description() const noexcept {
    return pimpl_->desc;
}

void MI::hash(Hasher& h) {
    // Only hash the value if it's opaque
    if(!pimpl_->is_transparent) h(pimpl_->value);
}

MI& MI::set_description(description_type desc) noexcept {
    pimpl_->desc = std::move(desc);
    return *this;
}

MI& MI::make_optional() noexcept {
    pimpl_->is_optional = true;
    return *this;
}

ModuleInput& ModuleInput::make_required() noexcept {
    pimpl_->is_optional = false;
    return *this;
}

ModuleInput& ModuleInput::make_transparent() noexcept {
    pimpl_->is_transparent = true;
    return *this;
}

ModuleInput& ModuleInput::make_opaque() noexcept {
    pimpl_->is_transparent = false;
    return *this;
}

const any_type& MI::get_() const { return pimpl_->value; }

void MI::change_(any_type new_value) {
    if(pimpl_->type == std::type_index(typeid(std::nullptr_t)))
        throw std::runtime_error("Must set type first");
    pimpl_->value.swap(new_value);
}

bool MI::is_valid_(const any_type& new_value) const {
    for(auto & [k, v] : pimpl_->checks)
        if(!v(new_value)) return false;
    return true;
}

void MI::set_type_(const std::type_info& type) noexcept {
    pimpl_->type = std::type_index(type);
}

MI& MI::add_check_(any_check check, description_type desc) {
    if(desc == "") {
        desc = "Check #";
        desc += std::to_string(pimpl_->checks.size());
    }
    pimpl_->checks.emplace(std::move(desc), std::move(check));
    return *this;
}

} // namespace SDE
