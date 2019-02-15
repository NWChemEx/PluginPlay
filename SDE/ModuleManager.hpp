#pragma once
#include "SDE/Module.hpp"
#include "SDE/ModuleBase.hpp"
#include "SDE/Types.hpp"
#include <memory>
namespace SDE {
namespace detail_ {
class ModuleManagerPIMPL;
}

class ModuleManager {
public:
    using module_base_ptr = std::shared_ptr<const ModuleBase>;

    ModuleManager();
    ModuleManager(const ModuleManager& rhs);
    ModuleManager& operator=(const ModuleManager& rhs) {
        return *this = std::move(ModuleManager(rhs));
    }
    ModuleManager(ModuleManager&& rhs) noexcept;
    ModuleManager& operator=(ModuleManager&& rhs) noexcept;
    ~ModuleManager() noexcept;

    type::size count(type::key key) const noexcept;

    void add_module(type::key module_key, module_base_ptr base);

    Module& at(const type::key& module_key) {
        const auto& rv = const_cast<const ModuleManager&>(*this).at(module_key);
        return const_cast<Module&>(rv);
    }
    const Module& at(const type::key& module_key) const;

    void copy_module(const type::key& old_key, type::key new_key);

    template<typename T>
    void set_default(type::key key) {
        set_default_(typeid(T), std::move(key));
    }

    template<typename T>
    void change_input(const type::key& key, const type::key& option,
                      T&& new_value) {
        at(key).change_input(option).change(std::forward<T>(new_value));
    }

    template<typename T, typename... Args>
    auto run_as(const type::key& key, Args&&... args) {
        return at(key).run_as<T>(std::forward<Args>(args)...);
    }

private:
    void set_default_(const std::type_info& type, type::key key);

    /// The object that actually implements the ModuleManager
    std::unique_ptr<detail_::ModuleManagerPIMPL> pimpl_;
}; // End class ModuleManager

} // namespace SDE
