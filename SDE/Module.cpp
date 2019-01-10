#include "SDE/Module.hpp"
#include "SDE/detail_/ModulePIMPL.hpp"

namespace SDE {
using input_type  = typename Module::input_type;
using input_map   = typename Module::input_map;
using output_type = typename Module::output_type;
using output_map  = typename Module::output_map;
using submod_type = typename Module::submod_type;
using submod_map  = typename Module::submod_map;

namespace detail_ {} // namespace detail_

Module::Module() : pimpl_(std::make_unique<detail_::ModulePIMPL>()) {}
Module::Module(const Module& rhs) :
  pimpl_(std::make_unique<detail_::ModulePIMPL>(*rhs.pimpl_)) {}
Module& Module::operator=(const Module& rhs) {
    std::make_unique<detail_::ModulePIMPL>(*rhs.pimpl_).swap(pimpl_);
    return *this;
}
Module::Module(Module&& rhs) noexcept = default;
Module& Module::operator=(Module&& rhs) noexcept = default;
Module::Module(std::unique_ptr<detail_::ModulePIMPL> pimpl) noexcept :
  pimpl_(std::move(pimpl)) {}
Module::~Module() = default;

output_map Module::run(input_map ps) const {
    return pimpl_->run(std::move(ps));
}

void Module::hash(bphash::Hasher& h) const {
    if(!is_ready()) throw std::runtime_error("Can not hash non-ready module");
    pimpl_->hash(h);
}

bool Module::is_ready() const { return true; }
const input_map& Module::inputs() const { return pimpl_->inputs(); }
const output_map& Module::outputs() const { return pimpl_->outputs(); }
const submod_map& Module::submods() const { return pimpl_->submods(); }

} // namespace SDE
