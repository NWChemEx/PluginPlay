#include "SDE/Module.hpp"
#include "SDE/detail_/ModulePIMPL.hpp"

namespace SDE {
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

type::result_map Module::run(type::input_map ps) {
    return pimpl_->run(std::move(ps));
}

void Module::hash(bphash::Hasher& h) const { pimpl_->hash(h); }
void Module::lock() noexcept { pimpl_->lock(); }

bool Module::ready() const noexcept { return pimpl_->ready(); }
bool Module::locked() const noexcept { return pimpl_->locked(); }

const type::input_map& Module::inputs() const noexcept {
    return pimpl_->inputs();
}
const type::result_map& Module::results() const { return pimpl_->results(); }
const type::submodule_map& Module::submods() const noexcept {
    return pimpl_->submods();
}

ModuleInput& Module::change_input(type::key key) {
    return pimpl_->inputs().at(key);
}

SubmoduleRequest& Module::change_submod(type::key key) {
    return pimpl_->submods().at(key);
}

bool Module::operator==(const SDE::Module& rhs) const {
    return *pimpl_ == *rhs.pimpl_;
}

void Module::unlock() noexcept { pimpl_->unlock(); }

} // namespace SDE
