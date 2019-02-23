#pragma once
#include "SDE/ModuleInput.hpp"
#include "SDE/ModuleResult.hpp"
#include "SDE/Types.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>

namespace SDE {

// Needed b/c Module and SubmoduleRequest use each other in their declarations
class SubmoduleRequest;

namespace detail_ {
class ModulePIMPL;
class ModuleManagerPIMPL;
} // namespace detail_

/** @brief The public API of all modules.
 *
 *  Developers implement classes that derive from ModuleBase. ModuleBase acts as
 *  a PIMPL (a ModuleBase instance is hidden inside the ModulePIMPL class)
 *  decoupling the algorithm details from the API. Users of the SDE interact
 *  with the algorithm through the common API implemented by the Module class.
 *
 *  The state of the Module class is divided in two. The first set of state is
 *  referred to as the algorithm's state and includes the list of allowed
 *  inputs, values of bound inputs, a list of all callback points, values of
 *  the submodules bound to those callback points, and the list of all possible
 *  results. This is state that is directly accessed during the execution of the
 *  algorithm, hence the name. The second set of state is termed module state
 *  and is needed by the SDE framework to correctly use the module. This state
 *  is not accessible (nor relevant) once the module is running. For the most
 *  part users will only be interested in algorithm state.
 */
class Module {
public:
    ///@{
    /** @name Ctors and Assignment Operators
     *
     *  A Module instance is only valid if it has been created by a
     *  ModuleManager instance or if it is created by copying/moving from a
     *  valid instance. Default constructed Modules are allowed as placeholders,
     *  but must be made valid by copy/move assignment. The validity of a module
     *  can be checked by calling the member `is_valid` (the similarly named
     *  `is_ready` can also be used for this purpose since `is_valid` is one of
     *  the checks it runs to ensure that the module is ready to be run).
     *
     *  @param rhs The instance we are constructing the module from. For the
     *         move ctor and assignment operators @p rhs will be in a valid, but
     *         otherwise undefined state after the operation.
     *  @param base An already made PIMPL instance for the class to use. In most
     *         cases this PIMPL will have been created by the ModuleManager.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate a new
     *         PIMPL (only relevant for 1, 2, and 3). Strong throw guarantee.
     *  @throw None Functions 4, 5, and 6 are no throw guarantee.
     */
    Module();
    Module(const Module& rhs);
    Module& operator=(const Module& rhs);
    Module(Module&& rhs) noexcept;
    Module& operator=(Module&& rhs) noexcept;
    Module(std::unique_ptr<detail_::ModulePIMPL> base) noexcept;
    ///@}

    /** @brief Standard destructor.
     *
     *  @throw none No throw guarantee.
     */
    ~Module() noexcept;

    /** @brief The primary API for running the encapsulated code.
     *
     *
     * @tparam property_type The class codifying the property type that the
     *         module should be run as.
     * @tparam Args The types of the input arguments
     * @param args The input values that will be forwarded to the
     * @return The property
     */
    template<typename property_type, typename... Args>
    auto run_as(Args&&... args) {
        auto temp = inputs();
        temp = property_type::wrap_inputs(temp, std::forward<Args>(args)...);
        return property_type::unwrap_results(run(temp));
    }

    /** @brief The advanced API for running the module.
     *
     *  This member allows you to set whatever inputs you would like and gives
     *  you access to all of the results. This flexibility comes at the cost of
     *  compile-time error checking. The result is that if you provide an input
     *  with an object of the incorrect type or try to cast an output to an
     *  incompatible type this will only be detected as an error at runtime
     *  when those commands are executed. All inputs will still be checked for
     *  domain errors per usual.
     *
     *  This function is primarily intended for use by developers who for
     *  whatever reason need to provide/access inputs/results that are not part
     *  of a property type. In most scenarios the use of this function signals
     *  the accrual of technical debt resulting from code coupling. The
     *  technical debt can be avoided by creating/modifying a property type so
     *  that it takes/returns the additional inputs/results.
     *
     *
     * @param ps A map from input keys to that input's values.
     * @return A map from output keys to that output's value.
     *
     * @throw ??? If the underlying algorithm throws, the exception will also be
     *        thrown by this function. Strong throw guarantee.
     */
    type::result_map run(type::input_map ps);

    ///@{
    /** @name Module state accessors
     *
     * The functions in this section are used to inquire about the state of the
     * module.
     *
     * Respectively these functions:
     *
     * - Whether a module is ready (contains an actual algorithm, all submodules
     *   are set, and all required options are set)
     * - Whether a module is locked
     *
     * @return The value of the requested piece of state.
     * @throw none No throw guarantee.
     */
    bool ready() const noexcept;
    bool locked() const noexcept;
    ///@}

    ///@{
    /** @name Module state modifiers
     *
     */
    void lock() noexcept;
    ///@}

    ///@{
    /** @name Algorithm state accessors
     *
     *  These functions allow you to view the state of the module in a read-only
     *  manner. Each function returns a map whose keys are all of the keywords
     *  recognized by the module in a particular context (as inputs, as results,
     *  or as submodules). For the inputs and submodules the values in the map
     *  are the objects that are currently bound to those keywords (*i.e.*, if
     *  the module was called right this second those are the values that each
     *  of those keywords would map to). The results map's values are null and
     *  will be filled in by the `run` function based on the values of the bound
     *  inputs and submodules as well as the inputs provided to the module. To
     *  change the values the inputs or submodules are bound to use
     *  `change_input` or `change_submodule` respectively.
     *
     *  @throw none All functions are no throw guarantee.
     */
    const type::input_map& inputs() const noexcept;
    const type::result_map& results() const;
    const type::submodule_map& submods() const noexcept;
    ///@}

    ///@{
    /** @name Algorithm state modifiers
     *
     * These functions allow you to change what value an input or submodule
     * keyword is bound to. Changes can only be made
     *
     * @param key Which input/submodule you want to modify.
     * @return The requested input/submodule.
     *
     * @throw std::out_of_range if @p key is not a valid key. Strong throw
     *        guarantee.
     *
     */
    ModuleInput& change_input(type::key key);
    SubmoduleRequest& change_submod(type::key key);
    ///@}

    /// Hashes the module
    void hash(type::hasher& h) const;

    ///@{
    /** @name Equality comparisons
     *
     * Two modules are equal if they both:
     *
     * - contain instances of the same ModuleBase class
     * - have the same bound input values
     * - have the same bound submodules
     * - are in the same locked state
     *
     * @param rhs The instance to compare against.
     * @return True if the comparison is true and false otherwise.
     * @throw ??? if any of the input value comparisons throw. Same throw
     *        guarantee.
     */
    bool operator==(const Module& rhs) const;
    bool operator!=(const Module& rhs) const { return !((*this) == rhs); }
    ///@}
private:
    friend class detail_::ModuleManagerPIMPL;

    /** @brief Unlocks a locked module
     *
     * There are very select circumstances when we need to unlock a locked
     * module (for example after deep copying a locked module, we need to
     * unlock the copy). The ModuleManager (through its PIMPL) is the only class
     * that can do this because it knows when it is okay.
     *
     * @throw none No throw guarantee.
     */
    void unlock() noexcept;

    /// The instance that actually does everything for us.
    std::unique_ptr<detail_::ModulePIMPL> pimpl_;
};

} // namespace SDE
