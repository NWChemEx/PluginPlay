#include "SDE/Module.hpp"

using module_pointer  = typename SDE::ModuleBase::module_pointer;
using submodules_list = typename SDE::ModuleBase::submodules_list;

namespace SDE {
namespace detail_ {

struct ModuleBasePIMPL {
    Utilities::CaseInsensitiveMap<module_pointer> submodules;

    // TODO: Add when SDE is written
    /// A handle to the framework
    // std::shared_ptr<SDE> sde_;

    // TODO: Add when Cache is written
    /// A collection of results previously obtained by this module type
    // std::shared_ptr<Cache> benjamins_;

    // TODO: Add when Parameters are written
    /// Values for the algorithmic parameters associated with the module
    // Parameters params_;

    void hash(Hasher& h) const { h(submodules); }
};

} // namespace detail_

ModuleBase::ModuleBase() :
  pimpl_(std::make_unique<detail_::ModuleBasePIMPL>()) {}

ModuleBase::~ModuleBase() noexcept {}

const submodules_list& ModuleBase::submodules() const noexcept {
    return pimpl_->submodules;
}

void ModuleBase::hash(Hasher& h) const { pimpl_->hash(h); }

void ModuleBase::swap(ModuleBase& rhs) noexcept { pimpl_.swap(rhs.pimpl_); }

} // namespace SDE
