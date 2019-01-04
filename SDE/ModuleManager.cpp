#include "SDE/ModuleManager.hpp"
#include "SDE/detail_/ModulePIMPL.hpp"

namespace SDE {

using key_type           = typename ModuleManager::key_type;
using module_base_ptr    = typename ModuleManager::module_base_ptr;
using submodule_requests = typename ModuleBase::submodule_requests;

namespace detail_ {
struct ModuleManagerPIMPL {
    Utilities::CaseInsensitiveMap<module_base_ptr> bases;
    Utilities::CaseInsensitiveMap<std::shared_ptr<Cache>> caches_;
    Utilities::CaseInsensitiveMap<Module> modules;
};
} // namespace detail_

void ModuleManager::add_module(key_type module_key, module_base_ptr base) {
    if(pimpl_->bases.count(module_key))
        throw std::invalid_argument("Module key: " + module_key +
                                    " already exists.");
    pimpl_->bases[module_key] = base;
    pimpl_->caches_.emplace(module_key, std::make_shared<Cache>());
    auto mod_pimpl = std::make_unique<ModulePIMPL>(caches_[module_key], base);
    pimpl_->modules.emplace(module_key, Module(std::move(mod_pimpl)));
}

void ModuleManager::copy_module(key_type old_key, key_type new_key){modules}

Module& ModuleManager::get_module(key_type module_key) {
    return pimpl_->modules.at(module_key);
}

} // namespace SDE
