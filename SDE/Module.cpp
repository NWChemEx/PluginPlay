#include "SDE/Module.hpp"

using module_pointer  = typename SDE::ModuleBase::module_pointer;
using submodules_list = typename SDE::ModuleBase::submodules_list;

inline submodules_list copy_submodules(const submodules_list& rhs) {
    submodules_list rv;
    for(const auto& x : rhs) {
        module_pointer new_ptr = x.second->clone();
        // Use hint insert b/c they will be in the same order...
        rv.insert(rv.end(), std::make_pair(x.first, std::move(new_ptr)));
    }
    return rv;
}

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

    ModuleBasePIMPL() = default;

    ModuleBasePIMPL(const ModuleBasePIMPL& rhs) :
      submodules(copy_submodules(rhs.submodules)) {}

    void hash(Hasher& h) const { h(submodules); }
};

} // namespace detail_

ModuleBase::ModuleBase() :
  pimpl_(std::make_unique<detail_::ModuleBasePIMPL>()) {}

ModuleBase::ModuleBase(const ModuleBase& rhs) :
  pimpl_(std::make_unique<detail_::ModuleBasePIMPL>(*rhs.pimpl_)) {}

ModuleBase& ModuleBase::operator=(const ModuleBase& rhs) {
    pimpl_ = std::make_unique<detail_::ModuleBasePIMPL>(*rhs.pimpl_);
    return *this;
}

ModuleBase::ModuleBase(ModuleBase&& rhs) noexcept :
  pimpl_(std::move(rhs.pimpl_)) {}

ModuleBase& ModuleBase::operator=(ModuleBase&& rhs) noexcept {
    pimpl_ = std::move(rhs.pimpl_);
    return *this;
}

ModuleBase::~ModuleBase() noexcept {}

const submodules_list& ModuleBase::submodules() const noexcept {
    return pimpl_->submodules;
}

void ModuleBase::hash(Hasher& h) const { pimpl_->hash(h); }

void ModuleBase::swap(ModuleBase& rhs) noexcept { pimpl_.swap(rhs.pimpl_); }

} // namespace SDE
