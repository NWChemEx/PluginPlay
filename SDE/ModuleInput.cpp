#include "SDE/ModuleInput.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <typeindex>

namespace SDE {
using any_check = typename ModuleInput::any_check;

namespace detail_ {

struct ModuleInputPIMPL {
    type::any value;
    Utilities::CaseInsensitiveMap<any_check> checks;
    std::type_index type   = std::type_index(typeid(std::nullptr_t));
    type::description desc = "";
    bool is_optional       = false;
    bool is_transparent    = false;
};

bool operator==(const ModuleInputPIMPL& lhs, const ModuleInputPIMPL& rhs) {
    return std::tie(lhs.value, lhs.type, lhs.desc, lhs.is_optional,
                    lhs.is_transparent) == std::tie(rhs.value, rhs.type,
                                                    rhs.desc, rhs.is_optional,
                                                    rhs.is_transparent);
}

} // namespace detail_

using MI = ModuleInput;

MI::ModuleInput() : pimpl_(std::make_unique<detail_::ModuleInputPIMPL>()) {}

MI::ModuleInput(const ModuleInput& rhs) :
  is_cref_(rhs.is_cref_),
  is_actually_cref_(rhs.is_actually_cref_),
  pimpl_(std::make_unique<detail_::ModuleInputPIMPL>(*rhs.pimpl_)) {}

MI& MI::operator=(const ModuleInput& rhs) {
    std::make_unique<detail_::ModuleInputPIMPL>(*rhs.pimpl_).swap(pimpl_);
    return *this;
}
MI::ModuleInput(ModuleInput&& rhs) noexcept = default;
MI& MI::operator=(ModuleInput&& rhs) noexcept = default;
MI::~ModuleInput() noexcept                   = default;

bool MI::is_optional() const noexcept { return pimpl_->is_optional; }
bool MI::is_transparent() const noexcept { return pimpl_->is_transparent; }
bool MI::is_ready() const noexcept {
    return is_optional() || pimpl_->value.has_value();
}
const type::description& MI::description() const noexcept {
    return pimpl_->desc;
}

void MI::hash(Hasher& h) const {
    // Only hash the value if it's opaque
    if(!pimpl_->is_transparent) h(pimpl_->value);
}

MI& MI::set_description(type::description desc) noexcept {
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

const type::any& MI::get_() const { return pimpl_->value; }

void MI::change_(type::any new_value) {
    if(pimpl_->type == std::type_index(typeid(std::nullptr_t)))
        throw std::runtime_error("Must set type first");
    pimpl_->value.swap(new_value);
}

bool MI::is_valid_(const type::any& new_value) const {
    for(auto & [k, v] : pimpl_->checks)
        if(!v(new_value)) return false;
    return true;
}

void MI::set_type_(const std::type_info& type) noexcept {
    pimpl_->type = std::type_index(type);
}

MI& MI::add_check_(any_check check, type::description desc) {
    if(desc == "") {
        desc = "Check #";
        desc += std::to_string(pimpl_->checks.size());
    }
    pimpl_->checks.emplace(std::move(desc), std::move(check));
    return *this;
}

bool MI::operator==(const ModuleInput& rhs) const {
    return *pimpl_ == *rhs.pimpl_;
}

} // namespace SDE
