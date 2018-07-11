#include "SDE/ModuleManager.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <iostream>
template<typename T>
using CIM = Utilities::CaseInsensitiveMap<T>;

namespace SDE {

// Forward some typedefs from the MM class
using key_type       = typename ModuleManager::key_type;
using module_pointer = typename ModuleManager::module_pointer;
using loader_type    = typename ModuleManager::loader_type;
using module_entry   = std::pair<module_pointer, loader_type>;
using size_type      = typename ModuleManager::size_type;

namespace detail_ {

// Member functions of this class implement the members of ModuleManager with
// the same names
class MMImpl {
private:
    // Used as a suffix to generate (hopefully) unique keys
    size_t number = 0;

    // Wraps the process of generating a valid key
    key_type generate_valid_key() {
        key_type key = "__A_UNIQUE_KEY__";
        do { key += std::to_string(number++); } while(modules.count(key));
        return key;
    }

    // Code factorization for checking if a key is valid
    void is_valid(const std::string& key) const {
        if(modules.count(key))
            throw std::range_error(key + " already exists.");
        else if(key == "")
            throw std::invalid_argument("key can not be empty.");
    }

public:
    // The actual modules
    CIM<module_entry> modules;

    void insert(key_type key, loader_type mod) {
        is_valid(key);
        auto mod_i = mod();
        auto entry = std::make_pair(mod(), mod);
        modules.insert(std::make_pair(std::move(key), entry));
    }

    key_type duplicate(const key_type& key1, key_type key2) {
        if(key2 == "")
            key2 = generate_valid_key();
        else
            is_valid(key2);
        auto da_pair     = modules.at(key1);
        auto loader      = da_pair.second;
        auto new_mod     = loader();
        (*new_mod)       = (*da_pair.first);
        new_mod->locked_ = false;
        auto entry       = std::make_pair(new_mod, std::move(loader));
        modules.insert(std::make_pair(key2, std::move(entry)));
        return key2;
    }

    module_pointer get_module(const key_type& key) {
        return modules.at(key).first;
    }
};

} // namespace detail_

ModuleManager::ModuleManager() : pimpl_(std::make_unique<detail_::MMImpl>()) {}

ModuleManager::ModuleManager(ModuleManager&& mm) noexcept :
  pimpl_(std::move(mm.pimpl_)) {}

ModuleManager& ModuleManager::operator=(ModuleManager&& mm) noexcept {
    pimpl_ = std::move(mm.pimpl_);
    return *this;
}

ModuleManager::~ModuleManager() noexcept {}

void ModuleManager::insert(key_type key, loader_type mod) {
    pimpl_->insert(std::move(key), std::move(mod));
}

size_type ModuleManager::count(const key_type& key) const noexcept {
    return pimpl_->modules.count(key);
}

key_type ModuleManager::duplicate(const key_type& key1, const key_type& key2) {
    return pimpl_->duplicate(key1, key2);
}

module_pointer ModuleManager::at(const key_type& key) {
    return pimpl_->get_module(key);
}

} // namespace SDE
