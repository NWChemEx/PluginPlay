/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "pluginplay/types.hpp"
#include <pluginplay/fields/fields.hpp>
#include <pluginplay/utility/uuid.hpp>
#include <utilities/containers/case_insensitive_map.hpp>

namespace pluginplay {

// Needed b/c Module and SubmoduleRequest use each other in their declarations
class SubmoduleRequest;
class ModuleBase;

namespace detail_ {
class ModulePIMPL;
class ModuleManagerPIMPL;
} // namespace detail_

/** @brief The public API of all modules.
 *
 *  Developers implement classes that derive from ModuleBase. ModuleBase acts as
 *  a PIMPL (a ModuleBase instance is hidden inside the ModulePIMPL class)
 *  decoupling the algorithm details from the API. Users of the pluginplay
 * interact with the algorithm through the common API implemented by the Module
 * class.
 *
 *  The state of the Module class is divided in two. The first set of state is
 *  referred to as the algorithm's state and includes the list of allowed
 *  inputs, values of bound inputs, a list of all callback points, values of
 *  the submodules bound to those callback points, and the list of all possible
 *  results. This is state that is directly accessed during the execution of the
 *  algorithm, hence the name. The second set of state is termed module state
 *  and is needed by the pluginplay framework to correctly use the module. This
 * state is not accessible (nor relevant) once the module is running. For the
 * most part users will only be interested in algorithm state.
 */
class Module {
public:
    /// Type of a pointer to a PIMPL
    using pimpl_ptr = std::unique_ptr<detail_::ModulePIMPL>;

    /// Type of the object used to convey why a module is not ready
    using not_ready_type = utilities::CaseInsensitiveMap<std::set<type::key>>;

    /// Type of UUIDs
    using uuid_type = utility::uuid_type;

    /// Type of a submodule to UUID map
    using submod_uuid_map = std::map<type::key, uuid_type>;

    /** @brief Makes a module with no implementation.
     *
     *  The Module instance resulting from this ctor wraps no algorithm, has no
     *  description, and is not locked. The only way to make the instance have
     *  an algorithm is to assign to it from an instance that already has an
     *  algorithm. In general working Module instances will be made by the
     *  ModuleManager and the only reason to make a Module instance without an
     *  algorithm is as a placeholder.
     *
     *  @throw none No throw guarantee.
     */
    Module();

    /** @brief Initializing this Module with a deep copy of @p rhs
     *
     *  This ctor will make a deep copy of @p rhs. The resulting Module instance
     *  will have the same state. Of note this includes locked-ness.
     *
     *  @param[in] rhs The instance to deep copy.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy @p rhs.
     *                        Strong throw guarantee.
     */
    Module(const Module& rhs);

    /** @brief Sets this Module's state to a deep copy of @p rhs
     *
     *  This function will set the current instance's state to a deep copy of
     *  @p rhs's state. The resulting Module have the same state as @p rhs
     *  including lockedness. Memory associated with the old state will be freed
     *  up and references will thus become invalid. The notable exception is
     *  the cache and the results in it, which will only be freed if they are
     *  not also in use elsewhere.
     *
     *  @param[in] rhs The instance to deep copy.
     *
     *  @return The current instance set to a deep copy of @p rhs.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy @p rhs.
     *                        Strong throw guarantee.
     */
    Module& operator=(const Module& rhs);

    /** @brief Initializes this Module with @p rhs's state
     *
     *  This ctor will take ownership of @p rhs's state. The resulting instance
     *  will have the same state, including locked-ness.
     *
     *  @param[in] rhs The instance to take the state of. After this function
     *                 call @p rhs is in a valid, but otherwise undefined state.
     *
     *  @throw none No throw guarantee.
     */
    Module(Module&& rhs) noexcept;

    /** @brief Sets this Module's state to @p rhs's state
     *
     *  This function will set the current instance's state to @p rhs's state.
     *  The resulting Module will have the same state as @p rhs including
     *  lockedness. Memory associated with the old state will be freed up and
     *  references will thus become invalid. The notable exception is the cache
     *  and the results in it, which will only be freed if they are not also in
     *  use elsewhere.
     *
     *  @param[in] rhs The instance to take the state of. After this operation
     *                 @p rhs is in a valid, but otherwise undefined state.
     *
     *  @return The current instance, now with @p rhs's state.
     *
     *  @throw none No throw guarantee.
     */
    Module& operator=(Module&& rhs) noexcept;

    /** @brief Creates a Module with the provided implementation
     *
     *  This ctor will create a new Module instance which is implemented by the
     *  provided implementation. This ctor is largely intended to be used for
     *  creating lambda modules or other "one-off" types of modules because it
     *  does **not** associate a cache with the Module. To associate a cache
     *  with the module one must use the ctor that takes a pointer to a
     *  ModulePIMPL instance.
     *
     *  @param[in] ptr The implementation the Module should use.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        PIMPL for the resulting instance. Strong throw
     *                        guarantee.
     */
    explicit Module(std::unique_ptr<ModuleBase> ptr);

    /** @brief Instantiates a Module with the provided backend.
     *
     *  This ctor is primarily designed to be used by the ModuleManager;
     *  however, it can also be used by consumers who do not want to use the
     *  ModuleManager.
     *
     * @param[in] base The backend and state that will be used to implement this
     *                 module.
     *
     * @throw none No throw guarantee.
     */
    explicit Module(pimpl_ptr base) noexcept;

    /** @brief Returns an unlocked deep copy of this module.
     *
     *  The copy ctor copies all aspects of the module including lockedness. If
     *  you want to change the state of a locked module you need to make an
     *  unlocked copy. This function will do that for you.
     *
     *  @return An unlocked deep copy of this module.
     *
     *  @throw std::bad_alloc if the copy ctor throws. Strong throw guarantee.
     */
    Module unlocked_copy() const;

    /** @brief Standard destructor.
     *
     *  After this call the cache and results may still persist depending on
     *  whether or not their reference counts went to zero upon deletion. Of
     *  note the ModuleManager usually holds a shared_ptr to the cache and the
     *  cache usually holds one to the result.
     *
     *  @throw none No throw guarantee.
     */
    ~Module() noexcept;

    /** @brief Does this module contain an implementation?
     *
     *  The ModuleBase class is responsible for implementing most of this class.
     *  The ModuleManager only takes ModuleBase instances and therefore always
     *  has modules with implementations; however, we allow users to default
     *  construct (and copy/move from default constructed instances) which means
     *  it is possible for a Module to not have an implementation. This
     *  function is used to check whether or not the current module has an
     *  implementation.
     *
     *  @return True if the module has an implementation and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_module() const noexcept;

    /** @brief Did the module developer set a description for this module?
     *
     *  This function is used to determine if the developer has set the module's
     *  description.
     *
     *  @return true if the description has been set and false otherwise.
     *
     *  @throw std::runtime_error if the current module does not have an
     *                            implementation. Strong throw guarantee.
     */
    bool has_description() const;

    /** @brief Is the current module locked?
     *
     *  A locked module can not have its state modified. This avoids situations
     *  that may invalidate memoization caused by an input changing after the
     *  memoization check occurred. This function is used to determine if the
     *  current module is locked or not.
     *
     *  @return true if the module is locked and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool locked() const noexcept;

    /** @brief Returns a list of module state that is not "ready"
     *
     *  Calling `ready` is an easy way to determine if the module's `run` member
     *  can be called; however, if the module is not ready it can be a bit of a
     *  pain to figure out why. This function will return a map containing the
     *  reasons why the module is not ready. The keys of the map describe the
     *  part of the module that is not ready. Choices are:
     *
     *  - "Inputs" to indicate that one or more required inputs have not been
     *    set
     *  - "Submodules" to indicate that one or more submodules have not been
     *  set.
     *
     *  For inputs and submodules, the value in the map is the set of
     *  input/submodule keys corresponding to the inputs/submodules that are
     *  not set yet.
     *
     *  Note that since some inputs will be provided via the property type it is
     *  possible for the module to be ready even if all the bound inputs are
     *  not. For this reason this function takes an optional list of inputs.
     *  These inputs are assumed to be part of the property type's API and will
     *  not be included in the value returned by this function.
     *
     * @param[in] in_inputs The set of inputs provided by the property type.
     *                      Defaults to an empty map. Values are not checked for
     *                      ready-ness.
     *
     * @return A map of this module's state that is not set yet.
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *                       returned object. Strong throw guarantee.
     * @throw std::runtime_error if the implementation of this module has not
     *                           been set. Strong throw guarantee.
     */
    not_ready_type list_not_ready(const type::input_map& in_inputs = {}) const;

    /** @brief Determines if this module is ready to be run as the provided
     *         property type
     *
     * This function is equivalent to calling `list_not_ready` and with the
     * inputs provided by property type @p PropertyType and returning whether or
     * not the returned list is empty.
     *
     * @tparam PropertyType The property type we want to know if the module can
     *                      be run as.
     *
     * @return True if the module is ready to be run as @p PropertyType and
     *         false otherwise.
     *
     * @throw std::runtime_error if an implementation has not been set yet.
     *                           Strong throw guarantee.
     * @throw std::bad_alloc if `list_not_ready` has insufficient memory. Strong
     *                       throw guarantee.
     */
    template<typename PropertyType>
    bool ready() const;

    /** @brief Determines if this module is ready to be run given the inputs
     *         provided.
     *
     * This function is equivalent to calling `list_not_ready` and with the
     * inputs provided and returning whether or not the returned list is empty.
     *
     * @param[in] inps The inputs to forward to list_not_ready.
     *
     * @return True if the module can be run given the inputs provided in
     *        @p inps and false otherwise.
     *
     * @throw std::runtime_error if an implementation has not been set yet.
     *                           Strong throw guarantee.
     * @throw std::bad_alloc if `list_not_ready` has insufficient memory. Strong
     *                       throw guarantee.
     */
    bool ready(const type::input_map& inps = type::input_map{}) const;

    /** @brief Resets cache.
     *
     *  This function will reset cache.
     *
     *  @warning This will result in losing all the data
     *  (for all instances of this module) stored in the cache.
     *
     */
    void reset_cache();

    /** @brief Resets the implementation internal cache.
     *
     *  This function will reset cache.
     *
     *  @warning This will result in losing all the data
     *  (for all instances of this module) stored in the internal cache.
     *
     */
    void reset_internal_cache();

    /** @brief Is the module memoizable?
     *
     *  Some modules (lambda_modules or modules that have nondetermenistic
     *  results) should not be memoized.
     *
     *  @return true if the module is memoizable, false otherwise.
     *
     *  @warning If the module doesn't have a cache, results will not be cached
     *  even if `is_memoizable` is true .
     *
     *  @throw std::runtime_error if the current module does not have an
     *                            implementation. Strong throw guarantee.
     */
    bool is_memoizable() const;

    /** @brief Turns off memoization for this module
     *
     * This function turns off memoization so that results will not be cached.
     * Memoization is on for all modules except lambda_modules by default.
     * Can be used for modules with nondeterministic results, or lambda_modules.
     *
     */
    void turn_off_memoization();

    /** @brief Turns on memoization for this module
     *
     *  @warning If the module doesn't have a cache, results will not be cached
     *  even if this function is called and `is_memoizable` is true .
     *
     *  @throw std::runtime_error if the current module does not have an
     *                            implementation. Strong throw guarantee.
     */
    void turn_on_memoization();

    /** @brief Locks the module and all submodules
     *
     *  A locked module can no longer have its inputs or submodules modified.
     *  This function will first lock all submodules (throwing if any of the
     *  submodules are not ready to be locked) and then lock the current module.
     *  Unlike the lock calls to the submodules, which know the type that the
     *  module will be run as, and can thus assert their readiness, this call
     *  does not know how the module will be run and will perform no such check
     *  for this module.
     *
     *  This call also locks all submodules
     *
     *  @throws std;:runtime_error if a submodule is not ready. Strong throw
     *                             guarantee.
     */
    void lock();

    /** @brief Returns the set of results that can be computed by this module.
     *
     *  The set of results that a module can compute is the union of the
     *  set of results each property type can compute with the set of additional
     *  results that the developer specified. This function will return the set
     *  of results that a module can compute. The resulting set will not contain
     *  any actual results, but rather simply serves as a list of what can be
     *  computed.
     *
     *  @return  The set of results that this module can compute.
     *
     *  @throw std::runtime_error if no implementation has been set. Strong
     *                            throw guarantee.
     */
    const type::result_map& results() const;

    /** @brief Returns the inputs this module recognizes.
     *
     *  A module can have input values bound to it. Ultimately in order for the
     *  module to run all required inputs must have a value bound to them. These
     *  values come from two places those passed to the run_as/run function and
     *  those bound to the module. This function returns the set of all inputs
     *  that this module expects as well as any bound values.
     *
     *  This function is for read-only access. Use change_input to bind a value
     *  or modify an already bound value.
     *
     * @return The set of inputs bound to this module.
     *
     * @throw std::runtime_error if the implementation of this module has not
     *                           been set. Strong throw guarantee.
     */
    const type::input_map& inputs() const;

    /** @brief Returns the submodule callback points defined by this module
     *
     *  Modules may define callback points where they will call other modules
     *  to compute an intermediate result. Associated with each of these
     *  callback points is a SubmoduleRequest. This function can be used to
     *  retrieve the list of submodule callback points defined by this module as
     *  well as which submodule is bound to that callback point.
     *
     * @return The set of submodule requests defined by this module.
     *
     * @throw std::runtime_error if the implementation of this module has not
     *                           been set. Strong throw guarantee.
     */
    const type::submodule_map& submods() const;

    /** @brief Returns the set of property types that this module can be run
     *         as.
     *
     *  The pluginplay will only run a module as a whitelisted property type.
     * When a developer specifies that a module satisfies a particular property
     * type that property type gets whitelisted. It is also possible for the
     * user to specify additional property types that a module satisfies. Since
     * the user can not modify the sets of inputs/results that the module
     *  takes/computes adding additional property types only makes sense if the
     *  module already satisfies the property type, but the developer did not
     *  say so. Regardless, this function can be used to retrieve the set of
     *  property types that this module can be run as.
     *
     * @return The set of property types that this module satisfies.
     *
     * @throw std::runtime_error if this instance does not have an
     *                           implementation. Strong throw guarantee.
     */
    const std::set<type::rtti>& property_types() const;

    /** @brief Returns the human-readable description provided by the developer
     *
     *  Developers are encouraged to provide human-readable descriptions of what
     *  their modules do. This description will be used as documentation. If
     *  the developer set a description then this function can be used to
     *  retrieve it.
     *
     *  @return The description that the developer set.
     *
     *  @throw std::runtime_error if this module does not have an
     *                            implementation. Strong throw guarantee.
     *  @throw std::bad_optional_access if the description was not set. Strong
     *                                  throw guarantee.
     *
     */
    const type::description& description() const;

    /** @brief Returns a list of things to cite if you use the module.
     *
     *  We intend for most of the modules that are used with the pluginplay to
     * be developed in an academic context. In those setting it's important to
     *  make sure credit is given where credit is due. This function will return
     *  a list of literature refrences that should be cited if you use this
     *  module.
     *
     *  @return A list of citations.
     *
     *  @throw std::runtime_error if this module does not have an implementation
     *                            set. Strong throw guarantee.
     */
    const std::vector<type::description>& citations() const;

    /** @brief Binds an input value to the specified input.
     *
     *  Inputs to a module can be set in one of two ways: by passing the inputs
     *  to run_as/run or by binding them to a module instance. This function is
     *  used to bind an input value to a module instance.
     *
     * @tparam T The type of value being bound. Must be convertible to the type
     *           of the input field.
     *
     * @param[in] key The input whose value is being bound to @p value.
     * @param[in] value The new value of this input.
     *
     * @throw std::runtime_error if the module's backend is not set or the
     *                           module is locked, Strong throw guarantee.
     * @throw std::out_of_range if @p key does not correspond to an existing
     *                          input. Strong throw guarantee.
     * @throw std::invalid_argument if @p value fails one or more bounds checks
     *                              including being the wrong type. Strong throw
     *                              guarantee.
     */
    template<typename T>
    void change_input(const type::key& key, T&& value);

    /** @brief Changes the specified submodule to the provided module.
     *
     *  This overload will register the submodule @p new_submod under the key
     *  @p key. This overload is a convenience overload that is useful when the
     *  submodule is not stored in the ModuleManager or when the user wants to
     *  break aliasing. Most of the time, when the submodule is in the
     *  ModuleManager one wants to use the overload of this function which takes
     *  a shared_ptr in order to keep the connections synchronized.
     *
     *  @param[in] key The key under which the module should be registered.
     *                 @p key must be one of the keys defined by this module's
     *                 implementation.
     *
     *  @param[in] new_mod The module to register under the provided key.
     *
     *  @throw std::runtime_error if this module does not have an implementation
     *                            or if it is locked, Strong throw guarantee.
     *  @throw std::out_of_range if @p key does not correspond to a recognized
     *                           callback. Strong throw guarantee.
     *  @throw std::invalid_argument if @p new_mod does not satisfy the property
     *                               type associated with @p key. Strong throw
     *                               guarantee.
     *  @throw std::bad_alloc if there is insufficient memory to wrap @p new_mod
     *                        in a shared_ptr. Strong throw guarantee.
     */
    void change_submod(type::key key, Module new_mod);

    /** @brief Allows the user to change the submodule a module uses.
     *
     *  Each module maintains a list of callback points that it will use. Users
     *  can bind a particular module to a particular callback point using this
     *  function.
     *
     * @param[in] key The callback point to bind the module to.
     * @param[in] new_module The module to bind to the callback point.
     *
     * @throw std::runtime_error if the module does not have an implementation
     *                           or if it is locked, Strong throw guarantee.
     * @throw std::out_of_range if @p key does not map to an existing callback
     *                          point. Strong throw guarantee.
     * @throw std::invalid_argument if @p new_module does not satisfy the
     *                              requested property type. Strong throw
     *                              guarantee.
     */
    void change_submod(type::key key, std::shared_ptr<Module> new_module);

    /** @brief Makes the module satisfy the specified property type.
     *
     *  Each property type defines a series of inputs and results that a module
     *  must accept and compute. It is possible that a module satisfies a
     *  property type, but the module's developer did not register that the
     *  module satisfies that type. This function can be used to add additional
     *  property types to a module. Note that because a user can not add
     *  additional inputs or results to the module, the module must already
     *  satisfy the property type to be added. This function does not check if
     *  the module is capable of satisfying the provided property type.
     *
     *  @tparam PropertyType The additional property type that this module
     * should satisfy.
     *
     *  @throw std::runtime_error if this module does not have a backend or if
     *                            the module is locked. Strong throw guarantee.
     */
    template<typename PropertyType>
    void add_property_type();

    /** @brief The primary API for running the encapsulated code.
     *
     *  When a user runs a module they typically go through this API. The input
     *  to this API are the property type to run the module as and the inputs
     *  defined by the property type. The results are the results defined by the
     *  property type.
     *
     * @tparam property_type The class codifying the property type that the
     *                       module should be run as. An error will be raised if
     *                       the module does not satisfy that property type.
     * @tparam Args The types of the input arguments. Must be implicitly
     *              convertible to the input types defined by the property type.
     *
     * @param[in] args The input values that will be forwarded to the module.
     *
     * @return the results defined by the property type.
     *
     * @throw std::runtime_error if the module does not have an implementation,
     *                           the provided inputs are not ready, if the
     *                           module is not ready, or if the module is not
     *                           of the specified property type. Strong throw
     *                           guarantee.
     */
    template<typename property_type, typename... Args>
    auto run_as(Args&&... args);

    /** @brief The advanced API for running the module.
     *
     *  This member allows you to set whatever inputs you would like and gives
     *  you access to all of the results. This flexibility comes at the cost of
     *  loosing compile-time type checking. The result is that if you provide an
     *  input with an object of the incorrect type or try to cast an output to
     *  an incompatible type this will only be detected as an error at runtime.
     *  All inputs will still be checked for domain errors per usual.
     *
     *  This function is primarily intended for use by developers who for
     *  whatever reason need to provide/access inputs/results that are not part
     *  of a property type. In most scenarios the use of this function signals
     *  the accrual of technical debt resulting from code coupling. The
     *  technical debt can be avoided by creating/modifying a property type so
     *  that it takes/returns the additional inputs/results.
     *
     * @param[in] ps A list of inputs and the values to bind to them.
     *
     * @return A map from output keys to that output's value.
     *
     * @throw std::runtime_error if the module does not have an implementation,
     *                           the provided inputs are not ready, or if the
     *                           module is not ready. Strong throw
     *                           guarantee.
     *
     * @throw ??? If the underlying algorithm throws. Strong throw guarantee.
     */
    type::result_map run(type::input_map ps = {});

    /** @brief Returns timing data for this module and all submodules.
     *
     *  Each time the run member is called the time for the call (including all
     *  pluginplay overhead) is recorded. This also occurs for all calls to
     * submodules' run members. This function creates a formatted string with
     * this module's timing data, including the breakdown in terms of submodule
     * calls.
     *
     *  @return All timing data collected for this module and its submodules as
     *          a formatted string.
     *
     *  @throw std::bad_alloc if there's insufficient memory to allocate the
     *         return. Strong throw guarantee.
     */
    std::string profile_info() const;

    submod_uuid_map submod_uuids() const;

    uuid_type uuid() const;

    /** @brief Compares two Module instances for equality
     *
     * Two modules are equivalent if they contain the same algorithm (determined
     * by comparing the `base` member), they have the same bound inputs, the
     * same bound set of submodules, the same lockedness, and satisfy the same
     * property types.
     *
     * @param[in] rhs The instance to compare against
     *
     * @return True if the instances are equal and false otherwise.
     *
     * @throws ??? If the base comparison throws or if any of the input
     *         comparisons throw. Same guarantee.
     */
    bool operator==(const Module& rhs) const;

    /** @brief Determines if two Module instances are different
     *
     * Two modules are equivalent if they contain the same algorithm (determined
     * by comparing the `base` member), they have the same bound inputs, the
     * same bound set of submodules, the same lockedness, and satisfy the same
     * property types.
     *
     * @param[in] rhs The instance to compare against.
     *
     * @return False if the instances are equal and false otherwise.
     *
     * @throws ??? If the base comparison throws or if any of the input
     *         comparisons throw. Same guarantee.
     */
    bool operator!=(const Module& rhs) const { return !((*this) == rhs); }

private:
    /** @brief Unlocks a locked module
     *
     *  There are very select circumstances when we need to unlock a locked
     *  module. This function will do it.
     *
     * @throw none No throw guarantee.
     */
    void unlock_() noexcept;

    /// Returns the inputs in a read/write state
    ModuleInput& get_input_(const type::key& key);

    /// Guts for adding a property type
    void add_property_type_(type::rtti prop_type);

    /// Code factorization for ensuring the module is not locked.
    void assert_not_locked_();

    /// Hides the check of the property type
    void check_property_type_(type::rtti prop_type);

    /// The instance that actually does everything for us.
    pimpl_ptr m_pimpl_;
}; // class Module

std::string print_not_ready(const Module& mod, const type::input_map& ps = {},
                            const std::string& indent = "");

//-----------------------------Implementations----------------------------------

inline Module Module::unlocked_copy() const {
    Module rv(*this);
    rv.unlock_();
    return rv;
}

template<typename PropertyType>
bool Module::ready() const {
    auto temp = PropertyType::inputs();
    type::input_map inps(temp.begin(), temp.end());
    return ready(inps);
}

template<typename T>
void Module::change_input(const type::key& key, T&& value) {
    assert_not_locked_();
    get_input_(key).change(std::forward<T>(value));
}

inline void Module::change_submod(type::key key, Module new_mod) {
    change_submod(std::move(key), std::make_shared<Module>(std::move(new_mod)));
}

template<typename PropertyType>
void Module::add_property_type() {
    assert_not_locked_();
    add_property_type_(type::rtti(typeid(PropertyType)));
}

template<typename property_type, typename... Args>
auto Module::run_as(Args&&... args) {
    check_property_type_(type::rtti{typeid(property_type)});
    auto temp = inputs();
    temp      = property_type::wrap_inputs(temp, std::forward<Args>(args)...);
    using r_type  = decltype(property_type::unwrap_results(run(temp)));
    using clean_t = std::decay_t<r_type>;
    if constexpr(std::is_same_v<clean_t, void>) {
        property_type::unwrap_results(run(temp));
    } else {
        auto rv = property_type::unwrap_results(run(temp));

        if constexpr(std::tuple_size_v<clean_t> == 1) {
            return std::get<0>(rv);
        } else {
            return rv;
        }
    }
}

inline void Module::assert_not_locked_() {
    if(locked()) throw std::runtime_error("Locked modules can not be modified");
}

} // namespace pluginplay
