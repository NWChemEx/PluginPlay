#include "SDE/Module.hpp"
#include "SDE/detail_/ModulePIMPL.hpp"

namespace SDE {
using input_map  = typename Module::input_map;
using result_map = typename Module::result_map;
using submod_map = typename Module::submod_map;

using pimpl = detail_::ModulePIMPL;

Module::Module() : pimpl_(std::make_unique<pimpl>()) {}
Module::Module(const Module& rhs) :
  pimpl_(std::make_unique<pimpl>(*rhs.pimpl_)) {}

Module& Module::operator=(const Module& rhs) {
    std::make_unique<detail_::ModulePIMPL>(*rhs.pimpl_).swap(pimpl_);
    return *this;
}
Module::Module(Module&& rhs) noexcept = default;
Module& Module::operator=(Module&& rhs) noexcept = default;
Module::Module(std::unique_ptr<detail_::ModulePIMPL> pimpl) noexcept :
  pimpl_(std::move(pimpl)) {}
Module::~Module() = default;

result_map Module::run(input_map ps) { return pimpl_->run(std::move(ps)); }

void Module::hash(bphash::Hasher& h) const { pimpl_->hash(h); }
void Module::lock() noexcept {}

bool Module::ready() const noexcept { return pimpl_->ready(); }
bool Module::locked() const noexcept { return pimpl_->is_locked(); }

const input_map& Module::inputs() const noexcept { return pimpl_->inputs(); }
const result_map& Module::results() const noexcept { return pimpl_->results(); }
const submod_map& Module::submods() const noexcept { return pimpl_->submods(); }

ModuleInput& Module::change_input(type::key key) {
    return pimpl_->inputs().at(key);
}

SubmoduleRequest& Module::change_submod(type::key key) {
    return pimpl_->submods().at(key);
}

} // namespace SDE
