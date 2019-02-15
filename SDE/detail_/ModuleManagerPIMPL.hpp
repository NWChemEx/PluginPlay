#pragma once
#include "SDE/ModuleBase.hpp"
#include "SDE/detail_/ModulePIMPL.hpp"
#include <memory>
#include <typeindex>

namespace SDE {
namespace detail_ {

/** @brief The class that implements the ModuleManager.
 *
 *  Users are expected to go through the API provided by the ModuleManager
 *  class. Inside the ModuleManager class calls are redirected to this class.
 *  When using this class calls should go through the member functions as much
 *  as possible so that error checking occurs. That said members are public so
 *  that it is easier to test the implementation.
 */
struct ModuleManagerPIMPL {
    ///@{
    /// Type of a pointer to a module's implemenation
    using module_base_ptr = std::shared_ptr<const ModuleBase>;

    /// Type of a map from the module implementation's type to the
    /// implementation
    using base_map = std::map<std::type_index, module_base_ptr>;

    /// Type of a usable module
    using shared_module = std::shared_ptr<Module>;

    /// Type of a map holding usable modules
    using module_map = Utilities::CaseInsensitiveMap<std::shared_ptr<Module>>;

    /// Type of a cache
    using cache_type = typename ModulePIMPL::cache_type;

    /// Type of a shared cache
    using shared_cache = std::shared_ptr<cache_type>;

    /// Type of a map holding caches
    using cache_map = std::map<std::type_index, shared_cache>;

    /// Type of a map holding the default module key for a given property type
    using default_map = std::map<std::type_index, type::key>;
    ///@}

    auto clone() { return std::make_unique<ModuleManagerPIMPL>(*this); }

    /// Ensures we determine if we have a module consistently
    type::size count(const type::key& key) const noexcept {
        return m_modules.count(key);
    }

    /// Ensures we count the number of modules consistently
    type::size size() const noexcept { return m_modules.size(); }

    void set_default(const std::type_info& type, type::key key) {
        if(!count(key)) m_modules.at(key); // Throws a consistent error
        m_defaults[std::type_index(type)] = key;
    }

    void add_module(type::key key, module_base_ptr base) {
        assert_unique_key_(key);

        std::type_index type(base->type());
        if(!m_bases.count(type)) m_bases[type] = base;
        if(!m_caches.count(type))
            m_caches[type] = std::make_shared<cache_type>();
        auto pimpl =
          std::make_unique<ModulePIMPL>(m_bases[type], m_caches[type]);
        auto ptr = std::make_shared<Module>(std::move(pimpl));
        m_modules.emplace(std::move(key), ptr);
    }

    void copy_module(const type::key& old_key, type::key new_key) {
        assert_unique_key_(new_key);
        Module mod(*m_modules.at(old_key));
        mod.unlock();
        auto ptr = std::make_shared<Module>(std::move(mod));
        m_modules.emplace(std::move(new_key), ptr);
    }

    shared_module at(const type::key& key) {
        auto mod = m_modules.at(key);
        // Loop over submodules filling them in from the defaults
        for(auto & [k, v] : mod->submods()) {
            const auto& type = v.type();
            // Only change non-ready submodules
            if(!v.ready() && m_defaults.count(type)) {
                // Recursive to make sure that that module gets filled in
                auto default_mod = at(m_defaults.at(type));
                // Only change if the module is also ready
                if(default_mod->ready())
                    mod->change_submod(k).change(default_mod);
            }
        }
        return mod;
    }

    ///@{
    /** @name Comparison operators
     *
     * @param rhs
     * @return
     */
    bool operator==(const ModuleManagerPIMPL& rhs) const {
        // Try to get out early
        if(m_bases.size() != rhs.m_bases.size()) return false;
        if(m_modules.size() != rhs.m_modules.size()) return false;
        if(m_defaults.size() != rhs.m_defaults.size()) return false;

        // Skip checking the values b/c implementations are compared by type
        for(const auto & [k, v] : rhs.m_bases) {
            if(!m_bases.count(k)) return false;
        }

        // Need to check the values b/c user may have switched options
        for(const auto & [k, v] : rhs.m_modules) {
            if(!m_modules.count(k)) return false;
            if(*m_modules.at(k) != *v) return false;
        }

        // Easy since not pointers
        if(m_defaults != rhs.m_defaults) return false;

        return true;
    }

    bool operator!=(const ModuleManagerPIMPL& rhs) const {
        return !((*this) == rhs);
    }
    ///@}

    ///@{
    /** @name ModuleManager state
     *
     *  The members in this section are the state of the ModuleManager class.
     */
    // These are the Modules in the states provided to us by the developer
    base_map m_bases;

    // These are the Modules in the state set by the user
    module_map m_modules;

    // These are the results of the modules running in the user's states
    cache_map m_caches;

    // A map of property types
    default_map m_defaults;
    ///@}
private:
    void assert_unique_key_(const type::key& key) const {
        if(count(key)) throw std::invalid_argument("Key is in use");
    }
};

} // namespace detail_
} // namespace SDE
