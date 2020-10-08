#pragma once
#include "sde/module_base.hpp"
#include "sde/types.hpp"
#include <iomanip> // for put_time
#include <utilities/timer.hpp>

namespace sde::detail_ {

// TODO: This function doesn't belong here (move unit test too)
/** @brief Creates a string whose contents is a time stamp.
 *
 *  C++ doesn't have a great way to get a time stamp as a string. This function
 *  wraps the process of making such a string. The resulting string contains
 *  both the date and the time (to second accuracy).
 *
 *  @return A std::string containing the current date and time in the format
 *          `<day>-<month>-<year> <hour>:<minute>:<second>`
 *
 *  @throw std::bad_alloc if there is insufficient memory to allocate the
 *         string. Strong throw guarantee.
 */
inline auto time_stamp() {
    // TODO: Check the returns of these two functions for errors
    auto t  = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::stringstream ss;
    ss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
    // TODO: Make sure ss's bad_bit wasn't set
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
    using base_ptr = std::shared_ptr<const ModuleBase>;

    /// Type of the object used to store the results computed by the module
    using cache_type = std::map<std::string, type::result_map>;

    /// How we store the object used for caching
    using cache_ptr = std::shared_ptr<cache_type>;

    /// How we tell you what's preventing your module from running
    using not_set_type = utilities::CaseInsensitiveMap<std::set<type::key>>;

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
    ModulePIMPL(const ModulePIMPL& rhs) = default;

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
    ModulePIMPL& operator=(const ModulePIMPL& rhs) = default;

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
     *  @throw none No throw guarantee.
     */
    void unlock() noexcept { m_locked_ = false; }

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
     *  The SDE will not implicitly convert a module. This function returns the
     *  list of property types that the SDE is allowed to convert the module to.
     *  The final list is the union of the property types whitelisted by the
     *  developer and those whitelisted by the user.
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
     *  The SDE will not implicitly convert a module. This function returns the
     *  list of property types that the SDE is allowed to convert the module to.
     *  The final list is the union of the property types whitelisted by the
     *  developer and those whitelisted by the user.
     *
     * @return The set of property types that this module satisfies.
     *
     * @throw std::runtime_error if this PIMPL does not have an implementation.
     *                           Strong throw guarantee.
     */
    auto& property_types() const;

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
     *  We intend for most of the modules that are used with the SDE to be
     *  developed in an academic context. In those setting it's important to
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

    /** @brief Computes the hash of the current instance using the bound params.
     *
     *  This function simply calls memoize with the currently bound inputs.
     *
     *  @param[in,out] h The hasher to use for hashing.
     *
     *  @throw ??? if any of the hash functions throw. Same throw
     */
    void hash(type::hasher& h) const { memoize(h, m_inputs_); }

    /** @brief computes a hash for a particular invocation of the `run` member.
     *
     *  For a deterministic module providing the module the same inputs must
     *  return the same outputs. We need a way to determine if we have already
     *  called the module with a particular set of inputs; that's where this
     *  function comes in. This function takes a set of input values, as well as
     *  the set of submodules to use, and maps them to a hash value. Barring the
     *  universe conspiring against us, that hash value is a concise and unique
     *  representation of the input state.
     *
     * @param[in,out] h The hasher instance to use
     * @param[in] inputs The values of the inputs to hash
     *
     * @throw std::bad_alloc if there is insufficient memory to merge the
     *                       inputs. Strong throw guarantee.
     * @throw ??? If the hash function of any input or submodule throws. Strong
     *            throw guarantee.
     */
    void memoize(type::hasher& h, type::input_map inputs) const;

    /** @brief Returns timing data for this module and all submodules.
     *
     *  Each time the run member is called the time for the call (including all
     *  SDE overhead) is recorded. This also occurs for all calls to submodules'
     *  run members. This function creates a formatted string with this module's
     *  timing data, including the breakdown in terms of submodule calls.
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
    
    /** @brief Resets cache.
     *
     *  This function will reset cache.
     *  
     *  @warning This will result in losing all the data
     *  (for all instances of this module) stored in the cache.
     *
     */    
    void reset_cache();

    /** @brief Is the module memoizable?
     *
     *  Some modules (lambda_modules or modules that have nondetermenistic 
     *  results) should not be memoized.
     *
     *  @return true if the module is memoizable, false otherwise.
     *
     *  @throw std::runtime_error if the current module does not have an
     *                            implementation. Strong throw guarantee.
     */
    bool is_memoizable() const;

    /** @brief Turn of memoization for this module
     *
     *  This function will disable memoization for this module. Note that 
     *  memoization is on for all modules except lambda_modules by default.
     *
     *  @throw std::runtime_error if the current module does not have an
     *                            implementation. Strong throw guarantee.
     */    
    void turn_off_memoization();

    /** @brief Turn of memoization for this module
     *
     *  This function will enable memoization for this module. Note that 
     *  memoization is on for all modules except lambda_modules by default.
     *
     *  @throw std::runtime_error if the current module does not have an
     *                            implementation. Strong throw guarantee.
     */    
    void turn_on_memoization();

    /** @brief Actually runs the module
     *
     *  This is the function with all of the SDE magic. Ultimately it will call
     *  the run function of the wrapped ModuleBase instance, but before it does
     *  that it ensures the module is ready to run, locks it, and checks the
     *  cache for the result. If the result is not found in the cache it finally
     *  calls the wrapped instance. After the call the result will be cached
     *  and returned.
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

private:
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

    /// Code factorization for computing the hash of a module
    std::string get_hash_(const type::input_map& in_inputs);

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

    /// Timer used to time runs of this module
    utilities::Timer m_timer_;
}; // class ModulePIMPL

//-------------------------------Implementations--------------------------------

inline ModulePIMPL::ModulePIMPL(base_ptr base, cache_ptr cache) :
  m_base_(base),
  m_cache_(cache),
  m_inputs_(base ? base->inputs() : type::input_map{}),
  m_submods_(base ? base->submods() : type::submodule_map{}),
  m_property_types_(base ? base->property_types() : std::set<type::rtti>{}) {}

inline bool ModulePIMPL::has_description() const {
    assert_mod_();
    return m_base_->has_description();
}

inline auto ModulePIMPL::not_set(const type::input_map& in_inputs) const {
    assert_mod_();
    not_set_type probs;

    // This is all of the not ready inputs
    auto in_probs = not_set_guts_(m_inputs_);

    // Now pull out those set by the property type
    for(const auto& [k, v] : in_inputs)
        if(in_probs.count(k)) in_probs.erase(k);
    if(!in_probs.empty()) probs.emplace("Inputs", std::move(in_probs));

    auto submod_probs = not_set_guts_(m_submods_);
    if(!submod_probs.empty())
        probs.emplace("Submodules", std::move(submod_probs));
    return probs;
}

inline bool ModulePIMPL::ready(const type::input_map& inps) const {
    auto errors = not_set(inps);
    return errors.empty();
}

inline const auto& ModulePIMPL::results() const {
    assert_mod_();
    return m_base_->results();
}

inline auto& ModulePIMPL::inputs() {
    assert_mod_();
    return m_inputs_;
}

inline auto& ModulePIMPL::inputs() const {
    assert_mod_();
    return m_inputs_;
}

inline auto& ModulePIMPL::submods() {
    assert_mod_();
    return m_submods_;
}

inline auto& ModulePIMPL::submods() const {
    assert_mod_();
    return m_submods_;
}

inline auto& ModulePIMPL::property_types() {
    assert_mod_();
    return m_property_types_;
}

inline auto& ModulePIMPL::property_types() const {
    assert_mod_();
    return m_property_types_;
}

inline auto& ModulePIMPL::description() const {
    assert_mod_();
    return m_base_->get_desc();
}

inline auto& ModulePIMPL::citations() const {
    assert_mod_();
    return m_base_->citations();
}

inline bool ModulePIMPL::is_memoizable() const {
    assert_mod_();
    auto memoizable = m_memoizable_;
    for(const auto& [k, v] : m_submods_){
        memoizable = v.value().is_memoizable() && memoizable;
    }
    return memoizable;
}

inline void ModulePIMPL::memoize(type::hasher& h,
                                 type::input_map inputs) const {
    inputs = merge_inputs_(std::move(inputs));
    for(const auto& [k, v] : inputs) v.hash(h);
    for(const auto& [k, v] : m_submods_) v.hash(h);
    // This is not a great way of hashing the class name...
    h(m_base_->type().name());
}

inline bool ModulePIMPL::is_cached(const type::input_map& in_inputs) {
    if(!m_cache_) return false;
    auto ps = merge_inputs_(in_inputs);
    return m_cache_->count(get_hash_(ps)) == 1;
}

inline void ModulePIMPL::reset_cache(){
    m_cache_.reset();
}

inline void ModulePIMPL::turn_off_memoization(){
    assert_mod_();
    m_memoizable_ = false;
}

inline void ModulePIMPL::turn_on_memoization(){
    assert_mod_();
    m_memoizable_ = true;
}

inline std::string ModulePIMPL::profile_info() const {
    std::stringstream ss;
    ss << m_timer_;
    std::string tab("  ");
    for(auto [key, submod] : m_submods_) {
        ss << tab << key << std::endl;
        auto submod_prof_info = submod.value().profile_info();
        std::stringstream ss2(submod_prof_info);
        std::string token;
        while(std::getline(ss2, token, '\n'))
            ss << tab << tab << token << std::endl;
    }
    return ss.str();
}

inline auto ModulePIMPL::run(type::input_map ps) {
    auto time_now = time_stamp();
    m_timer_.reset();
    assert_mod_();
    // Check the inputs we were just given
    for(const auto& [k, v] : ps)
        if(!v.ready()) throw std::runtime_error("Inputs are not ready");

    // Merge with bound and see if we are ready
    if(!ready(ps)) throw std::runtime_error("Module is not ready to be run");

    lock();

    ps = merge_inputs_(ps);
    // Check cache
    auto hv = get_hash_(ps);

    if(is_memoizable() && m_cache_ && m_cache_->count(hv)) {
        m_timer_.record(time_now);
        return m_cache_->at(hv);
    }

    // not there so run
    auto rv = m_base_->run(std::move(ps), m_submods_);
    if(!m_cache_ || !is_memoizable()) {
        m_timer_.record(time_now);
        return rv;
    }

    // cache result
    m_cache_->emplace(hv, std::move(rv));
    m_timer_.record(time_now);
    return m_cache_->at(hv);
}

inline bool ModulePIMPL::operator==(const ModulePIMPL& rhs) const {
    if(has_module() != rhs.has_module()) return false;
    if(locked() != rhs.locked()) return false;
    if(!has_module()) return true;

    if(std::tie(inputs(), submods(), property_types()) !=
       std::tie(rhs.inputs(), rhs.submods(), rhs.property_types()))
        return false;
    return (*m_base_ == *rhs.m_base_);
}

inline type::input_map ModulePIMPL::merge_inputs_(
  type::input_map in_inputs) const {
    for(const auto& [k, v] : m_inputs_)
        if(!in_inputs.count(k)) in_inputs[k] = v;
    return in_inputs;
}

inline void ModulePIMPL::lock() {
    for(auto& [k, v] : m_submods_) v.lock();
    m_locked_ = true;
}

/// Code factorization for computing the hash of a module
inline std::string ModulePIMPL::get_hash_(const type::input_map& in_inputs) {
    type::hasher h(bphash::HashType::Hash128);
    memoize(h, in_inputs);
    return bphash::hash_to_string(h.finalize());
}

template<typename T>
std::set<type::key> ModulePIMPL::not_set_guts_(T&& map) const {
    std::set<type::key> probs;
    for(const auto& [k, v] : map)
        if(!v.ready()) probs.insert(k);
    return probs;
}

inline void ModulePIMPL::assert_mod_() const {
    if(has_module()) return;
    throw std::runtime_error("Module does not contain an implementation");
}

} // namespace sde::detail_
