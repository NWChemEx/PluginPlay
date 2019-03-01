#pragma once
#include "SDE/Module.hpp"
#include "SDE/ModuleBase.hpp"
#include "SDE/Types.hpp"
#include <memory>
namespace SDE {
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

} // namespace SDE
