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
#include "fingerprint_table.hpp"
#include <chrono>
#include <iomanip> // for put_time
#include <optional>
#include <pluginplay/cache/module_cache.hpp>
#include <pluginplay/cache/module_manager_cache.hpp>
#include <pluginplay/driver/driver_base.hpp>
#include <pluginplay/module/module_base.hpp>
#include <pluginplay/types.hpp>
#include <utilities/timer.hpp>
#include <vector>

namespace pluginplay::detail_ {

// TODO: This function doesn't belong here (move unit test too)
/** @brief Creates a string whose contents is a time stamp.
 *
 *  C++ doesn't have a great way to get a time stamp as a string. This function
 *  wraps the process of making such a string. The resulting string contains
 *  both the date and the time (to millisecond accuracy).
 *
 *  @return A std::string containing the current date and time in the format
 *          `<day>-<month>-<year> <hour>:<minute>:<second>`
 *
 *  @throw std::bad_alloc if there is insufficient memory to allocate the
 *         string. Strong throw guarantee.
 */
inline auto time_stamp() {
    using namespace std::chrono;
    const auto now    = system_clock::now();
    const auto now_tt = system_clock::to_time_t(now);
    const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_tt), "%d-%m-%Y %H:%M:%S") << '.'
       << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

/** @brief The class that actually contains a module's state.
 *
 *  This class contains a module's actual state in the sense that whenever the
 *  module is called the values in this class are taken to be the bound values
 *  for the inputs, submodules, and property types. Of particular note is it is
 *  these values, and not the values in the developer-provided ModuleBase
 *  instance, that are used. This allows us to preserve the developer's default
 *  state while allowing users to override it.
 *
 *  As an implementation note, we choose to punt locked checks to the Module
 *  class itself. This is because this is best done in templated functions,
 *  which would prohibit us from using a PIMPL.
 *
 */
class ModulePIMPL {
public:
    /// Type used to store the developer-provided implementation
    using base_ptr = std::shared_ptr<ModuleBase>;

    /// Type of the object used to store the results computed by the module
    using cache_type = cache::ModuleCache;

    /// How we store the object used for caching
    using cache_ptr = std::shared_ptr<cache_type>;

    /// How we tell you what's preventing your module from running
    using not_set_type = utilities::CaseInsensitiveMap<std::set<type::key>>;

    /// Type of object containing RTTI of ModuleBase implementation
    using rtti_type = type::rtti;

    /// Type of the UUID
    using uuid_type = typename ModuleBase::uuid_type;

    /// Type of the submodule key to UUID map
    using submod_uuid_map = std::map<std::string, uuid_type>;

    /// Type of a pointer to the runtime the implementation uses
    using runtime_ptr = typename ModuleBase::runtime_ptr;

    /// Type of a pointer to the ModuleManager's caches
    using mm_cache_ptr = std::shared_ptr<cache::ModuleManagerCache>;

    /// Type of a pointer to the table assigning fingerprints
    using fingerprint_table_ptr = std::shared_ptr<FingerprintTable>;

    /// Type of a fingerprint
    using fingerprint_type = FingerprintTable::fingerprint_type;

    /// Type of a pointer to a module, e.g., an overlay module
    using module_ptr = std::shared_ptr<Module>;

    /// Type of the list of a driver's inactive settings
    using inactive_list = std::vector<std::string>;

    /** @brief Makes a module with no implementation.
     *
     *  The ModulePIMPL instance resulting from this ctor wraps no algorithm,
     *  has no description, and is not locked. The only way to make the instance
     *  have an algorithm is to assign to it from an instance that already has
     *  an algorithm. In general working Module instances will be made by the
     *  ModuleManager and the only reason to make a Module instance without an
     *  algorithm is as a placeholder.
     *
     *  @throw none No throw guarantee.
     */
    ModulePIMPL() = default;

    /** @brief Initializing this ModulePIMPL with a deep copy of @p rhs
     *
     *  This ctor will make a deep copy of @p rhs. The resulting ModulePIMPL
     *  instance will have the same state. Of note this includes locked-ness.
     *
     *  @param[in] rhs The instance to deep copy.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy @p rhs.
     *                        Strong throw guarantee.
     */
    ModulePIMPL(const ModulePIMPL& rhs);

    /** @brief Sets this ModulePIMPL's state to a deep copy of @p rhs
     *
     *  This function will set the current instance's state to a deep copy of
     *  @p rhs's state. The resulting ModulePIMPL have the same state as @p rhs
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
    ModulePIMPL& operator=(const ModulePIMPL& rhs);

    /** @brief Initializes this ModulePIMPL with @p rhs's state
     *
     *  This ctor will take ownership of @p rhs's state. The resulting instance
     *  will have the same state, including locked-ness.
     *
     *  @param[in] rhs The instance to take the state of. After this function
     *                 call @p rhs is in a valid, but otherwise undefined state.
     *
     *  @throw none No throw guarantee.
     */
    ModulePIMPL(ModulePIMPL&& rhs) = default;

    /** @brief Sets this ModulePIMPL's state to @p rhs's state
     *
     *  This function will set the current instance's state to @p rhs's state.
     *  The resulting ModulePIMPL have the same state as @p rhs
     *  including lockedness. Memory associated with the old state will be freed
     *  up and references will thus become invalid. The notable exception is
     *  the cache and the results in it, which will only be freed if they are
     *  not also in use elsewhere.
     *
     *  @param[in] rhs The instance to take the state of. After this operation
     *                 @p rhs is in a valid, but otherwise undefined state.
     *
     *  @return The current instance, now with @p rhs's state.
     *
     *  @throw none No throw guarantee.
     */
    ModulePIMPL& operator=(ModulePIMPL&& rhs) = default;

    /** @brief Makes a ModulePIMPL that wraps the provided algorithm
     *
     *  This is the main useful ctor for the ModulePIMPL class. It take a
     *  pointer to a ModuleBase instance (which is guaranteed to have a
     *  algorithm inside it) and optionally the cache that should be used for
     *  memoization.
     *
     * @param[in] base The ModulBase instance this PIMPL is wrapping. The state
     *                 (inputs, results, etc.) of the module will stem from this
     *                 instance.
     * @param[in] cache The object to use for memoization. Default is a nullptr.
     *
     * @throw std::bad_alloc if there is insufficient memory to copy the
     *                       developer's default state. Strong throw guarantee.
     */
    explicit ModulePIMPL(base_ptr base, cache_ptr cache = cache_ptr{});

    /** @brief Frees up the state bound to this module.
     *
     *  After this call the cache and results may still persist depending on
     *  whether or not their reference counts went to zero upon deletion. Of
     *  note the ModuleManager usually holds a shared_ptr to the cache and the
     *  cache usually holds one to the result.
     *
     *  @throw none No throw guarantee.
     */
    ~ModulePIMPL() = default;

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
    bool has_module() const noexcept { return static_cast<bool>(m_base_); }

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
    bool locked() const noexcept { return m_locked_; }

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
    auto not_set(const type::input_map& in_inputs = {}) const;

    /** @brief Determines if this module is ready to be run
     *
     * This function wraps `not_set` and simply asserts that no problems come
     * back.
     *
     * @param[in] inps The inputs to forward to `not_set`.
     *
     * @return True if the module is ready and false otherwise
     *
     * @throw std::runtime_error if an implementation has not been set yet.
     *                           Strong throw guarantee.
     * @throw std::bad_alloc if `not_set` has insufficient memory. Strong throw
     *                      guarantee.
     */
    bool ready(const type::input_map& inps = {}) const;

    /** @brief Locks the module and all submodules
     *
     *  A locked module can no longer have its inputs or submodules modified.
     *  This function will first lock all submodules (throwing if any of the
     *  submodules are not ready to be locked) and then lock the current module.
     *  Unlike the calls to the submodules, which know the type that the
     *  module will be run as
     *
     *  @throws std;:runtime_error if a submodule is not ready. Strong throw
     *                             guarantee.
     */
    void lock();

    /** @brief  Unlocks the module
     *
     *  This will not unlock the submodules because we can not do that safely.
     *  This function is only used internally within the Module class. Users are
     *  not allowed to unlock modules.
     *
     *  Unlocking a driver discards the overlays its requests were resolved
     *  into, so that the requests are resolved again when it is next locked.
     *  Unlocking an overlay module makes it an ordinary module, with copies of
     *  the inputs it referred to.
     *
     *  @throw std::bad_alloc if there is a problem copying an overlay's
     *                        inputs. Weak throw guarantee.
     */
    void unlock();

    /** @brief Returns the set of results computed by this module.
     *
     *  A module's results are the union of the results of each property type
     *  it satisfies as well as any additional results the developer specified.
     *  This set can not change once the developer has established it since
     *  adding an additional result requires modifying the algorithm to return
     *  an additional result. This is why we do not allow the user to modify
     *  the result fields. This function will return the set of results that a
     *  module can compute. The resulting set will not contain any results, but
     *  simply serves as a list of what will be computed.
     *
     *  @return  The set of results that this module can compute.
     *
     *  @throw std::runtime_error if no implementation has been set. Strong
     *                            throw guarantee.
     */
    const auto& results() const;

    /** @brief Returns the inputs bound to this module in a read/write state.
     *
     *  The inputs bound to a module are the union of those specified by the
     *  developer and any additional bound values set by the user. This function
     *  returns those inputs in a read/write state.
     *
     * @return The set of inputs bound to this module.
     *
     * @throw std::runtime_error if the implementation of this module has not
     *                           been set. Strong throw guarantee.
     */
    auto& inputs();

    /** @brief Returns the inputs bound to this module in a read-only state.
     *
     *  The inputs bound to a module are the union of those specified by the
     *  developer and any additional bound values set by the user. This function
     *  returns those inputs in a read-only state.
     *
     * @return The set of inputs bound to this module.
     *
     * @throw std::runtime_error if the implementation of this module has not
     *                           been set. Strong throw guarantee.
     */
    auto& inputs() const;

    /** @brief Returns the submodules bound to this module in a read/write
     *        state.
     *
     *  The submodules bound to a module are the union of those specified by the
     *  developer and any additional bound values set by the user. This function
     *  returns those submodule requests in a read/write state.
     *
     * @return The set of submodule requests bound to this module.
     *
     * @throw std::runtime_error if the implementation of this module has not
     *                           been set. Strong throw guarantee.
     */
    auto& submods();

    /** @brief Returns the submodules bound to this module in a read-only state
     *
     *  The submodules bound to a module are the union of those specified by the
     *  developer and any additional bound values set by the user. This function
     *  returns those submodule requests in a read-only state.
     *
     * @return The set of submodule requests bound to this module.
     *
     * @throw std::runtime_error if the implementation of this module has not
     *                           been set. Strong throw guarantee.
     */
    auto& submods() const;

    /** @brief Returns the set of property types that this module can be run
     *         as in a read/write state.
     *
     *  The pluginplay will not implicitly convert a module. This function
     * returns the list of property types that the pluginplay is allowed to
     * convert the module to. The final list is the union of the property types
     * whitelisted by the developer and those whitelisted by the user.
     *
     * @return The set of property types that this module satisfies.
     *
     * @throw std::runtime_error if this PIMPL does not have an implementation.
     *                           Strong throw guarantee.
     */
    auto& property_types();

    /** @brief Returns the set of property types that this module can be run
     *         as in a read-only state.
     *
     *  The pluginplay will not implicitly convert a module. This function
     * returns the list of property types that the pluginplay is allowed to
     * convert the module to. The final list is the union of the property types
     * whitelisted by the developer and those whitelisted by the user.
     *
     * @return The set of property types that this module satisfies.
     *
     * @throw std::runtime_error if this PIMPL does not have an implementation.
     *                           Strong throw guarantee.
     */
    auto& property_types() const;

    /** @brief Returns the set of Python-only property types (by name) that
     *         this module can be run as, in a read/write state.
     *
     *  @return The set of Python-only property type names that this module
     *          satisfies.
     *
     * @throw std::runtime_error if this PIMPL does not have an implementation.
     *                           Strong throw guarantee.
     */
    auto& python_property_types();

    /** @brief Returns the set of Python-only property types (by name) that
     *         this module can be run as, in a read-only state.
     *
     *  @return The set of Python-only property type names that this module
     *          satisfies.
     *
     * @throw std::runtime_error if this PIMPL does not have an implementation.
     *                           Strong throw guarantee.
     */
    auto& python_property_types() const;

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
    auto& description() const;

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
    auto& citations() const;

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

    /** @brief Checks whether the result of a call is cached.
     *
     *  This function will memoize the provided inputs and determine if the
     *  cache holds the result of calling this module. If this object does not
     *  have a cache then the result is falsed.
     *
     *  @param[in] in_inputs The inputs to use in the memoization
     *
     *  @return True if the cache has the value and false otherwise.
     */
    bool is_cached(const type::input_map& in_inputs);

    /** @brief Associates *this with a ModuleManager's runtime and caches.
     *
     *  Modules are created without a runtime, UUID, or caches. When a
     *  ModuleManager takes ownership of a module, it calls this function to
     *  give the module the ModuleManager's runtime and caches. The
     *  implementation is given a UUID if it does not already have one, and its
     *  user cache is the one associated with that UUID. Implementations shared
     *  by more than one module (*e.g.*, copies of a module) thus share a UUID
     *  and a user cache.
     *
     *  @param[in] runtime The runtime the implementation should use.
     *  @param[in] caches The ModuleManager's caches. If null, *this is not
     *                    given caches.
     *  @param[in] cache_key The key of the module cache *this should use.
     *  @param[in] fingerprints The table *this assigns fingerprints with. If
     *                          null, *this keeps its current table.
     *
     *  @throw std::runtime_error if *this does not have an implementation.
     *                            Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating a cache. Weak
     *                        throw guarantee.
     */
    void attach(runtime_ptr runtime, const mm_cache_ptr& caches,
                const type::key& cache_key,
                fingerprint_table_ptr fingerprints = {});

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
     *  @throw std::runtime_error if this module does not have an implementation
     *                            set. Strong throw guarantee.
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
     *  This function will disable memoization for this module. Note that
     *  memoization is on for all modules except lambda_modules by default.
     *
     *  @throw std::runtime_error if the current module does not have an
     *                            implementation. Strong throw guarantee.
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

    /** @brief Actually runs the module
     *
     *  This is the function with all of the pluginplay magic. Ultimately it
     * will call the run function of the wrapped ModuleBase instance, but before
     * it does that it ensures the module is ready to run, locks it, and checks
     * the cache for the result. If the result is not found in the cache it
     * finally calls the wrapped instance. After the call the result will be
     * cached and returned.
     *
     * @param[in] ps The input parameters set by the user.
     *
     * @return Whatever the module returns.
     *
     * @throw std::runtime_error if the module does not have an implementation,
     *                           the provided inputs are not ready, or if the
     *                           module is not ready. Strong throw
     *                           guarantee.
     * @throw ??? If the module throws.
     */
    auto run(type::input_map ps);

    /** @brief Compares two ModulePIMPL instances for equality
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
    bool operator==(const ModulePIMPL& rhs) const;

    /** @brief Determines if two ModulePIMPL instances are different
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
    bool operator!=(const ModulePIMPL& rhs) const { return !((*this) == rhs); }

    /** @brief Returns the RTTI of the class implementing the Module.
     *
     *  The ModuleManager uses the RTTI as a unique way to refer to the module's
     *  implementation. This function allows access to the RTTI through the
     *  PIMPL.
     *
     *  @return The RTTI of the implementation.
     *
     *  @throw std::runtime_error if the ModulePIMPL does not have an
     *                            implementation. Strong throw guarantee.
     */
    rtti_type type() const;

    /** @brief Returns the UUID assigned to the underlying algorithm.
     *
     *  When ModuleBase instances are added to the ModuleManager they are
     *  assigned UUIDs. This method allows you to retrieve the UUID the module
     *  was assigned.
     *
     *  @return The UUID assigned to the wrapped implementation. An empty string
     *          is returned if the wrapped implementation was not assigned a
     *          UUID.
     *
     *  @throws std::runtime_error if the ModulePIMPL does not have an
     *                             implementation. Strong throw guarantee.
     */
    uuid_type uuid() const;

    submod_uuid_map submod_uuids() const;

    // -------------------------------------------------------------------------
    // -- Drivers
    // -------------------------------------------------------------------------

    /** @brief Is *this a driver?
     *
     *  @return True if the implementation derives from DriverBase and false
     *          otherwise.
     *
     *  @throw std::runtime_error if *this does not have an implementation.
     *                            Strong throw guarantee.
     */
    bool is_driver() const;

    /// The developer's request, empty for modules which are not drivers
    const CallGraphRequest& developer_request() const;

    /// The user's request, empty for modules which are not drivers
    CallGraphRequest& user_request() noexcept { return m_user_request_; }

    /// The user's request, empty for modules which are not drivers
    const CallGraphRequest& user_request() const noexcept {
        return m_user_request_;
    }

    /// The inputs as the developer declared them, before the user's changes
    const type::input_map& default_inputs() const {
        assert_mod_();
        return m_base_->inputs();
    }

    /// Does *this memoize, regardless of the modules below it?
    bool memoizes() const noexcept { return m_memoizable_; }

    /// Have the requests of the driver *this been resolved?
    bool is_resolved() const noexcept { return m_resolved_.has_value(); }

    /** @brief Stores what the requests of the driver *this resolved into.
     *
     *  Called by the ModuleManager when it resolves the requests of *this.
     *
     *  @param[in] run_submods The submodule map *this runs with, which binds
     *                         its submodules to overlay modules where needed.
     *  @param[in] overlays The overlay modules reachable from @p run_submods,
     *                      which *this owns.
     *  @param[in] inactive The settings of *this which had no effect.
     *
     *  @throw None No throw guarantee.
     */
    void set_resolution(type::submodule_map run_submods,
                        std::vector<module_ptr> overlays,
                        inactive_list inactive) noexcept;

    /** @brief The submodules *this runs with.
     *
     *  For a driver whose requests are resolved, these are the submodules
     *  bound to the overlay modules. Otherwise they are submods().
     *
     *  If *this is a resolved driver which is not locked, the maps of its
     *  overlays are rebuilt from their originals first (see sync_overlay_).
     *
     *  @return The submodules *this runs with.
     *
     *  @throw std::bad_alloc if there is a problem rebuilding the maps of the
     *                        overlays. Weak throw guarantee.
     */
    const type::submodule_map& run_submods() const;

    /// The settings of a resolved driver which had no effect
    const inactive_list& inactive_settings() const noexcept;

    // -------------------------------------------------------------------------
    // -- Overlay modules
    // -------------------------------------------------------------------------

    /** @brief Makes an overlay module of @p original.
     *
     *  An overlay module points to @p original and stores only the inputs,
     *  submodules, and memoization setting that differ from it. Reading an
     *  input it does not change reads the original's value, without copying
     *  it. It shares the original's implementation and caches.
     *
     *  @param[in] original The module to overlay. Must have an implementation.
     *
     *  @return The new overlay module, which is unlocked and changes nothing.
     *
     *  @throw std::runtime_error if @p original has no implementation. Strong
     *                            throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the overlay.
     *                        Strong throw guarantee.
     */
    static module_ptr make_overlay(const module_ptr& original);

    /// Is *this an overlay module?
    bool is_overlay() const noexcept { return m_overlay_.has_value(); }

    /// The module *this overlays. Throws if *this is not an overlay module
    const Module& original() const;

    /** @brief Sets the input @p key of the overlay module *this.
     *
     *  @param[in] key The input to set.
     *  @param[in] value The new value of the input.
     *
     *  @throw std::runtime_error if *this is not an unlocked overlay module.
     *                            Strong throw guarantee.
     *  @throw std::out_of_range if *this has no input @p key. Strong throw
     *                           guarantee.
     *  @throw std::invalid_argument if @p value fails the input's checks.
     *                               Strong throw guarantee.
     */
    void override_input(const type::key& key, type::any value);

    /** @brief Binds the submodule @p key of the overlay module *this.
     *
     *  @param[in] key The submodule to set.
     *  @param[in] mod The module to bind.
     *
     *  @throw std::runtime_error if *this is not an unlocked overlay module,
     *                            or if @p mod does not satisfy the
     *                            submodule's property type. Strong throw
     *                            guarantee.
     *  @throw std::out_of_range if *this has no submodule @p key. Strong
     *                           throw guarantee.
     */
    void override_submod(const type::key& key, module_ptr mod);

    /** @brief Sets whether the overlay module *this memoizes.
     *
     *  @param[in] on Whether *this should memoize.
     *
     *  @throw std::runtime_error if *this is not an unlocked overlay module.
     *                            Strong throw guarantee.
     */
    void override_memoization(bool on);

    /** @brief Identifies the state of *this and of everything below it.
     *
     *  Two modules have the same fingerprint exactly when they, and the
     *  modules below them, would run with the same state, i.e., the same
     *  implementation, bound inputs, memoization setting, and modules below
     *  them. Fingerprints replace the UUIDs of the submodules in memoization
     *  keys. The fingerprint is stored once *this, and everything below it,
     *  is locked.
     *
     *  @return The fingerprint of *this.
     *
     *  @throw std::runtime_error if *this does not have an implementation.
     *                            Strong throw guarantee.
     */
    fingerprint_type fingerprint() const;

private:
    /// What an overlay module stores, on top of a ModulePIMPL
    struct OverlayState {
        /// The module the overlay points to
        module_ptr original;

        /// The inputs the overlay changes
        type::input_map input_overrides;

        /// The submodules the overlay changes
        type::submodule_map submod_overrides;

        /// The memoization setting, if the overlay changes it
        std::optional<bool> memoizes;
    };

    /// What the requests of a driver resolved into
    struct DriverState {
        /// The submodule map the driver runs with
        type::submodule_map run_submods;

        /// The overlay modules the driver owns
        std::vector<module_ptr> overlays;

        /// The settings of the driver which had no effect
        inactive_list inactive;
    };

    /// The submodules *this runs with (see run_submods)
    type::submodule_map& active_submods_();

    /// Rebuilds the maps of the overlay *this from its original and changes
    void sync_overlay_();

    /// Syncs the overlays of an unlocked, resolved driver, see sync_overlay_
    void sync_overlays_() const;

    /// Throws unless *this is an unlocked overlay module
    void assert_unlocked_overlay_() const;

    /// Computes the fingerprint, and whether it can be stored
    fingerprint_type fingerprint_(bool& is_final) const;

    /// Locks the module an overlay points to, as far as it can be locked
    static void lock_original_(Module& original);

    /** @brief Code factorization for merging two sets of inputs.
     *
     *  There are a couple scenarios where this class needs to merge inputs,
     *  such as when we are trying to determine if a particular set of inputs
     *  leads to a memoized result. This function factors the merging out into
     *  the present function.
     *
     *  @param[in] in_inputs The set of inputs to merge with this instance's
     *                       bound inputs.
     *
     *  @return The union of the inputs bound to this module and @p in_inputs.
     *
     *  @throw std::bad_alloc if there is insufficient memory to merge the two
     *                        sets of inputs. Strong throw guarantee.
     */
    type::input_map merge_inputs_(type::input_map in_inputs) const;

    /// Code factorization for checking if things in a map are ready
    template<typename T>
    std::set<type::key> not_set_guts_(T&& map) const;

    /// Code factorization for asserting that we have a module pointer
    void assert_mod_() const;

    /// Is the current module locked or not?
    bool m_locked_ = false;

    /// Is the current module memoizable?
    bool m_memoizable_ = true;

    /// The object actually implementing the algorithm
    base_ptr m_base_;

    /// The object used to cache calls to this module
    cache_ptr m_cache_;

    /// The inputs bound to this module
    type::input_map m_inputs_;

    /// The submodules bound to this module
    type::submodule_map m_submods_;

    /// The set of property types that his module satisfies
    std::set<type::rtti> m_property_types_;

    /// The names of the Python-only property types this module satisfies
    std::set<std::string> m_python_property_types_;

    /// Timer used to time runs of this module
    utilities::Timer m_timer_;

    /// The user's call graph request, for drivers
    CallGraphRequest m_user_request_;

    /// What the requests of a driver resolved into, once they are resolved
    std::optional<DriverState> m_resolved_;

    /// The state of an overlay module, if *this is one
    std::optional<OverlayState> m_overlay_;

    /// The table fingerprints are assigned with
    mutable fingerprint_table_ptr m_fingerprints_;

    /// The fingerprint, once it can no longer change
    mutable std::optional<fingerprint_type> m_fingerprint_;
}; // class ModulePIMPL

} // namespace pluginplay::detail_

#include "module_pimpl.ipp"
