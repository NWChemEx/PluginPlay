#pragma once
#include "sde/module.hpp"
#include "sde/module_base.hpp"
#include "sde/types.hpp"
#include <memory>
namespace sde {

namespace detail_ {
class ModuleManagerPIMPL;
}

/** @brief Class responsible for manipulating
 *
 */
class ModuleManager {
public:
    /// The type used to take in a new module
    using module_base_ptr = std::shared_ptr<const ModuleBase>;

    ///@{
    /** @name Ctors and assignment operators
     *
     */
    ModuleManager();
    ModuleManager(const ModuleManager& rhs);
    ModuleManager& operator=(const ModuleManager& rhs) {
        return *this = std::move(ModuleManager(rhs));
    }
    ModuleManager(ModuleManager&& rhs) noexcept;
    ModuleManager& operator=(ModuleManager&& rhs) noexcept;
    ///@}

    /// Standard dtor
    ~ModuleManager() noexcept;

    /** @brief Used to see if we have a module
     *
     * @param key
     * @return
     */
    type::size count(type::key key) const noexcept;

    template<typename ModuleType>
    void add_module(type::key module_key);

    /** @brief Used to add a new module to the list
     *
     * @param module_key
     * @param base
     */

    void add_module(type::key module_key, module_base_ptr base);

    ///@{
    /** @name Module retrievers
     *
     * @param module_key
     * @return
     */
    Module& at(const type::key& module_key) {
        const auto& rv = const_cast<const ModuleManager&>(*this).at(module_key);
        return const_cast<Module&>(rv);
    }
    const Module& at(const type::key& module_key) const;
    ///@}

    /** @brief Makes a "deep copy" of a module
     *
     * @param old_key
     * @param new_key
     */
    void copy_module(const type::key& old_key, type::key new_key);

    /** @brief Defines the default module for a particular property type
     *
     *  @tparam T The type of the property type the default is for
     *  @param key The module key of the default module
     */
    template<typename T>
    void set_default(type::key key) {
        auto temp = T::inputs();
        type::input_map inps(temp.begin(), temp.end());
        set_default_(typeid(T), std::move(inps), std::move(key));
    }

    /** @brief Changes the value of an input bound to a module
     *
     * @tparam T
     * @param key
     * @param option
     * @param new_value
     */
    template<typename T>
    void change_input(const type::key& key, const type::key& option,
                      T&& new_value) {
        at(key).change_input(option, std::forward<T>(new_value));
    }

    /** @brief Changes the submodule a module calls.
     *
     *  Modules can call other modules at predesignated callback points. Each of
     *  these callback points has a key associated with it. This function allows
     *  the user to change which module is called at a particular callback point
     *  by specifying the key of the module, the callback point's key, and the
     *  key for tne new submodule.
     *
     * @param[in] module_key   The key for the module whose submodule is being
     *                         changed.
     * @param[in] callback_key The name of the callback point in the module
     *                         assigned to @p module_key
     * @param[in] submod_key   The key for the submodule to call.
     *
     * @throw std::out_of_range if any of the keys do not refer to valid,
     *                          already present modules or callback points. Weak
     *                          throw guarantee.
     */
    void change_submod(const type::key& module_key,
                       const type::key& callback_key,
                       const type::key& submod_key);

    /** @brief Runs a given module
     *
     * @tparam T
     * @tparam Args
     * @param key
     * @param args
     * @return
     */
    template<typename T, typename... Args>
    auto run_as(const type::key& key, Args&&... args) {
        return at(key).run_as<T>(std::forward<Args>(args)...);
    }

private:
    /// Bridges the gap between the set_default and the PIMPL
    void set_default_(const std::type_info& type, type::input_map inps,
                      type::key key);

    /// The object that actually implements the ModuleManager
    std::unique_ptr<detail_::ModuleManagerPIMPL> pimpl_;
}; // End class ModuleManager

//------------------------------------------------------------------------------
//                          Inline Implementations
//------------------------------------------------------------------------------

template<typename ModuleType>
void ModuleManager::add_module(type::key module_key) {
    add_module(std::move(module_key), std::make_shared<ModuleType>());
}

} // namespace sde
