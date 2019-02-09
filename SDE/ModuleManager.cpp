#include "SDE/ModuleManager.hpp"
#include "SDE/detail_/ModulePIMPL.hpp"
#include <typeindex>
namespace SDE {

using key_type           = typename ModuleManager::key_type;
using module_base_ptr    = typename ModuleManager::module_base_ptr;
using submodule_requests = typename ModuleBase::submod_request;
using size_type          = typename ModuleManager::size_type;
using cache_type         = typename detail_::ModulePIMPL::cache_type;
template<typename T>
using CIM = Utilities::CaseInsensitiveMap<T>;

namespace detail_ {
struct ModuleManagerPIMPL {
    // These are the Modules in the states provided to us by the developer
    CIM<module_base_ptr> bases;

    // These are the Modules in the state set by the user
    CIM<Module> modules;

    // These are the results of the modules running in the user's states
    CIM<std::shared_ptr<cache_type>> caches;

    // A map of property types
    std::map<std::type_index, key_type> defaults;
};
} // namespace detail_

ModuleManager::ModuleManager() :
  pimpl_(std::make_unique<detail_::ModuleManagerPIMPL>()) {}
ModuleManager::ModuleManager(const ModuleManager& rhs) :
  pimpl_(std::make_unique<detail_::ModuleManagerPIMPL>(*rhs.pimpl_)) {}
ModuleManager& ModuleManager::operator=(const ModuleManager& rhs) {
    std::make_unique<detail_::ModuleManagerPIMPL>(*rhs.pimpl_).swap(pimpl_);
    return *this;
}
ModuleManager::ModuleManager(ModuleManager&& rhs) noexcept = default;
ModuleManager& ModuleManager::operator=(ModuleManager&& rhs) noexcept = default;
ModuleManager::~ModuleManager() noexcept                              = default;

size_type ModuleManager::count(key_type key) const noexcept {
    return pimpl_->bases.count(key);
}

void ModuleManager::add_module(key_type module_key, module_base_ptr base) {
    if(count(module_key))
        throw std::invalid_argument("Module key: " + module_key +
                                    " already exists.");
    pimpl_->bases[module_key] = base;
    pimpl_->caches.emplace(module_key, std::make_shared<cache_type>());
    const auto pcache = pimpl_->caches[module_key];
    auto mod_pimpl    = std::make_unique<detail_::ModulePIMPL>(base, pcache);
    pimpl_->modules.emplace(module_key, Module(std::move(mod_pimpl)));
}

void ModuleManager::copy_module(key_type old_key, key_type new_key) {
    add_module(new_key, pimpl_->bases.at(old_key));

    const auto& old_mod = at(old_key);
    auto& new_mod       = at(new_key);

    // new_mod.inputs()    = old_mod.inputs();
    // new_mod.submods()   = old_mod.submods();
}

Module& ModuleManager::at(key_type module_key) {
    return pimpl_->modules.at(module_key);
}
void ModuleManager::set_default_(const std::type_info& type, key_type key) {
    pimpl_->defaults[std::type_index(type)] = key;
}
} // namespace SDE
