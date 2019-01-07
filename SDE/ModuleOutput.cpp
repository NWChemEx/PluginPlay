#include "SDE/ModuleOutput.hpp"
#include <typeindex>

namespace SDE {

using shared_any       = typename ModuleOutput::shared_any;
using description_type = typename ModuleOutput::description_type;

namespace detail_ {

struct ModuleOutputPIMPL {
    shared_any value;
    description_type desc = "";
    std::type_index type  = std::type_index(typeid(std::nullptr_t));
};

} // namespace detail_

using MO = ModuleOutput;

MO::ModuleOutput() : pimpl_(std::make_unique<detail_::ModuleOutputPIMPL>()) {}
MO::ModuleOutput(const MO& rhs) :
  pimpl_(std::make_unique<detail_::ModuleOutputPIMPL>(*rhs.pimpl_)) {}
MO& MO::operator=(const MO& rhs) {
    std::make_unique<detail_::ModuleOutputPIMPL>(*rhs.pimpl_).swap(pimpl_);
    return *this;
}
MO::ModuleOutput(MO&& rhs) noexcept = default;
MO& MO::operator=(MO&& rhs) noexcept = default;
MO::~ModuleOutput() noexcept         = default;

const description_type& MO::description() const noexcept {
    return pimpl_->desc;
}

MO& MO::set_type_(const std::type_info& rtti) noexcept {
    pimpl_->type = std::type_index(rtti);
    return *this;
}

const shared_any& MO::at_() const noexcept { return pimpl_->value; }

void MO::change_(any_type new_value) {
    if(pimpl_->type == std::type_index(typeid(std::nullptr_t)))
        throw std::runtime_error("Call set_type with a valid type first.");
    if(std::type_index(new_value.type()) != pimpl_->type)
        throw std::invalid_argument("New value is incorrect type");
    change_(std::make_shared<any_type>(std::move(new_value)));
}

void MO::change_(shared_any new_value) noexcept { pimpl_->value = new_value; }

MO& MO::set_description(SDE::ModuleOutput::description_type desc) noexcept {
    pimpl_->desc = std::move(desc);
    return *this;
}
} // namespace SDE
