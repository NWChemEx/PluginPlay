#pragma once
#include "SDE/Module.hpp"
#include "SDE/ModuleBase.hpp"
#include <memory>
namespace SDE {
namespace detail_ {
class ModuleManagerPIMPL;
}

class ModuleManager {
public:
    using key_type        = std::string;
    using module_base_ptr = std::shared_ptr<const ModuleBase>;
    using size_type       = std::size_t;
    using module_type     = Module;

    ModuleManager();
    ModuleManager(const ModuleManager& rhs);
    ModuleManager& operator=(const ModuleManager& rhs);
    ModuleManager(ModuleManager&& rhs) noexcept;
    ModuleManager& operator=(ModuleManager&& rhs) noexcept;
    ~ModuleManager() noexcept;

    size_type count(key_type key) const noexcept;

    void add_module(key_type module_key, module_base_ptr base);

    module_type& at(key_type module_key);

    void copy_module(key_type old_key, key_type new_key);

    template<typename T>
    void set_default(key_type key) {
        set_default_(typeid(T), key);
    }

    template<typename T>
    void change_input(key_type key, key_type option, T&& new_value) {
        at(key).change_input(option).change(std::forward<T>(new_value));
    }

    template<typename T, typename... Args>
    auto run_as(key_type key, Args&&... args) {
        return at(key).run_as<T>(std::forward<Args>(args)...);
    }

private:
    void set_default_(const std::type_info& type, key_type key);

    /// The object that actually implements the ModuleManager
    std::unique_ptr<detail_::ModuleManagerPIMPL> pimpl_;
}; // End class ModuleManager

} // namespace SDE
