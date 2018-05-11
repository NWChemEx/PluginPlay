#include "SDE/ModuleManager.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>

template<typename T>
using CIM = Utilities::CaseInsensitiveMap<T>;

namespace SDE {

// Forward some typedefs from the MM class
using key_type       = typename ModuleManager::key_type;
using loader_type    = typename ModuleManager::loader_type;
using module_pointer = typename ModuleManager::module_pointer;

// Make some internal typedefs
using submod_map    = CIM<std::string>;
using shared_loader = std::shared_ptr<loader_type>;
using module_entry  = std::tuple<bool, shared_loader, submod_map>;

namespace detail_ {

// Member functions of this class implement the members of ModuleManager with
// the same names
class MMImpl {
private:
    // Used as a suffix to generate (hopefully) unique keys
    size_t number = 0;

    // Wraps the process of locking a module
    void lock_module(const key_type& key) {
        lock(key) = true; // at throws if not valid key
        for(const auto& kv : submods(key)) lock_module(kv.second);
    }

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

    // Wrappers to decouple from tuple position
    //{
    bool& lock(const key_type& key) { return std::get<0>(modules.at(key)); }

    shared_loader loader(const key_type& key) {
        return std::get<1>(modules.at(key));
    }

    module_pointer module(const key_type& key) {
        return loader(key)->get_module();
    }

    CIM<std::string>& submods(const key_type& key) {
        return std::get<2>(modules.at(key));
    }
    //}

    void insert(key_type key, const loader_type& loader) {
        is_valid(key);
        shared_loader ploader = std::move(loader.clone());
        submod_map subs;
        subs.swap(ploader->submodules);
        module_entry entry(false, ploader, std::move(subs));
        modules.insert(std::make_pair(std::move(key), std::move(entry)));
    }

    key_type duplicate(const key_type& key1, key_type key2) {
        if(key2 == "")
            key2 = generate_valid_key();
        else
            is_valid(key2);
        module_entry entry(false, loader(key1), submods(key1));
        modules.insert(std::make_pair(key2, std::move(entry)));
        return key2;
    }

    module_pointer get_module(const key_type& key) {
        if(!lock(key)) lock_module(key);
        auto rv = module(key);
        for(const auto& kv : submods(key)) { // set-up submodules
            auto modi = get_module(kv.second);
            auto kvi  = std::make_pair(kv.first, std::move(modi));
            rv->submodules_.insert(std::move(kvi));
        }
        return rv;
    }
};

} // namespace detail_

ModuleManager::ModuleManager() : pimpl_(std::make_unique<detail_::MMImpl>()) {}

ModuleManager::ModuleManager(ModuleManager&& mm) :
  pimpl_(std::move(mm.pimpl_)) {}

ModuleManager& ModuleManager::operator=(ModuleManager&& mm) {
    pimpl_ = std::move(mm.pimpl_);
    return *this;
}

ModuleManager::~ModuleManager() noexcept {}

void ModuleManager::insert(key_type key, const loader_type& loader) {
    pimpl_->insert(std::move(key), loader);
}

bool ModuleManager::count(const key_type& key) const noexcept {
    return pimpl_->modules.count(key);
}

key_type ModuleManager::duplicate(const key_type& key1, const key_type& key2) {
    return pimpl_->duplicate(key1, key2);
}

module_pointer ModuleManager::get_module(const key_type& key) {
    return pimpl_->get_module(key);
}

bool ModuleManager::is_locked(const key_type& key) const {
    return pimpl_->lock(key);
}

} // namespace SDE
