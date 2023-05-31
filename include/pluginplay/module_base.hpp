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
#include <memory>
#include <parallelzone/runtime/runtime_view.hpp>
#include <pluginplay/cache/cache.hpp>
#include <pluginplay/fields/fields.hpp>
#include <pluginplay/python/python_wrapper.hpp>
#include <pluginplay/submodule_request.hpp>
#include <pluginplay/utility/uuid.hpp>
#include <utilities/containers/case_insensitive_map.hpp>

namespace pluginplay {

/** @brief API determining how the pluginplay interacts with a module's
 * implementation
 *
 *  Module implementations derive from the ModuleBase class, which itself is
 *  used as a PIMPL for the Module class. When users interact with a module they
 *  do so with the Module class. Hence Module is the user API and ModuleBase is
 *  the developer API.
 *
 *  ModuleBase is meant to be inherited from. In order to avoid the user
 *  having to perform polymorphic copies/moves correctly, we simply disable
 *  copying/moving ModuleBase objects. In order to portray standard C++
 *  object behavior to the outside world the state of the module is copied
 *  into the ModulePIMPL class and copies/moves deal with that state. This
 *  also has the advantage of preserving the developer's original settings in
 *  the ModuleBase class while allowing the user to set their own defaults in
 *  the ModulePIMPL class (although they do it through the Module class). The
 *  default ctor is deleted to avoid developers erroneously forgetting to set
 *  the type (this is done implicitly by passing the this pointer).
 */
class ModuleBase {
private:
    /// Top-level type for the entire ModuleManager
    using mm_cache = cache::ModuleManagerCache;

public:
    /// Type of the UUID assigned to this module
    using uuid_type = utility::uuid_type;

    /// The type of the internal cache
    using cache_type = typename mm_cache::user_cache_type;

    /// A pointer to a cache
    using cache_ptr = typename mm_cache::user_cache_pointer;

    /// The type of the runtime
    using runtime_type = parallelzone::runtime::RuntimeView;

    /// A pointer to a runtime
    using runtime_ptr = std::shared_ptr<runtime_type>;

    /// Deleted to avoid errors
    ModuleBase() = delete;

    /// Deleted to avoid erroneous construction
    ModuleBase(const ModuleBase& rhs) = delete;

    /// Deleted to avoid erroneous assignment
    ModuleBase& operator=(const ModuleBase& rhs) = delete;

    /// Deleted to avoid erroneous construction
    ModuleBase(ModuleBase&& rhs) = delete;

    /// Deleted to avoid erroneous assignment
    ModuleBase& operator=(ModuleBase&& rhs) = delete;

    /** @brief Standard dtor
     *
     *  After calling this function all references to the state are invalidated.
     *  Of note, however, is that results will only be deleted if the reference
     *  count within the enclosing `shared_ptr` instance goes to zero.
     *
     *  @throw none No throw guarantee.
     */
    virtual ~ModuleBase() = default;

    /** @brief pluginplay-facing API for running the module.
     *
     * This function is used by the pluginplay to run the module. Values
     * provided to it have already been validated, and memoization has been
     * attempted. Ultimately all calls to run a module funnel to this function.
     *
     * @param[in] inputs The values the module should use as input.
     * @param[in] submods The submodules the module should use.
     *
     * @return Whatever quantities the module computes, in a type-erased form.
     *
     * @throw ??? if the module throws. Strong throw guarantee because all
     *            inputs are copies.
     */
    type::result_map run(type::input_map inputs,
                         type::submodule_map submods) const;

    /** @brief Used to determine if the module's description has been set.
     *
     *  Developers are encouraged to provide a human-readable description of
     *  what their module does. This string will then be used to generate
     *  documentation and help.
     *
     *  @return True if the developer has set a description and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_description() const noexcept { return m_desc_.has_value(); }

    /** @brief Retrieves the results this module can compute.
     *
     *  This function does **NOT** return actual computed results,, but rather a
     *  result_map of the results that can be computed with this module. When
     *  you run the module results are returned directly to you, there is no
     *  need to call this function unless you want to know ahead of time what
     *  results to expect.
     *
     *  This function also serves as a convenient mechanism for developers to
     *  construct their module's return value. Namely call this function, copy
     *  the result, set the copy's values, return the copy.
     *
     *  It does not make sense for the user to add additional results to a
     *  module and the list returned through Module::results will always be the
     *  same as the list returned by this function.
     *
     *  @return The set of results that the developer defined this module as
     *          being able to compute.
     *
     *  @throw none No throw guarantee
     */
    const type::result_map& results() const noexcept { return m_results_; }

    type::result_map& results() noexcept { return m_results_; }

    /** @brief Retrieves the inputs this module requires
     *
     *  This function can be used to retrieve the set of inputs that the
     *  developer defined. These are the results as they were defined in the
     *  module's ctor and thus are thus always the defaults. Hence using this
     *  function to inputs during run_ is a mistake (and will likely fail
     *  because some values are not set).
     *
     *  The set of inputs returned by Module::inputs is the set of inputs from
     *  this function combined with the defaults set by the user.
     *
     *  @return The set of inputs that the developer defined.
     *
     *  @throw none No throw guarantee.
     */
    const type::input_map& inputs() const noexcept { return m_inputs_; }

    type::input_map& inputs() noexcept { return m_inputs_; }
    /** @brief Returns the set of submodules requested by the developer.
     *
     *  This function returns the set of submodule requests that were created in
     *  the module's ctor. These requests are in the same state they were
     *  defined in, which means they are also unlikely to be fulfilled. Hence,
     *  using this function to retrieve submodules during your module's run_
     *  member is unlikely to work because you will not get any submodules.
     *
     *  The set of submodules returned by Module::submods is the set returned
     *  by this function, combined with any defaults set by the user.
     *
     *  @return The set of submodules specified by the developer in the ctor.
     *
     *  @throw none No throw guarantee.
     */
    const type::submodule_map& submods() const noexcept { return m_submods_; }

    type::submodule_map& submods() noexcept { return m_submods_; }

    /** @brief Returns the set of property types this module satisfies.
     *
     *  This function returns the set of property types that the developer
     *  okay'd their module to run as. It is used as the seed for the
     *  return of the similarly named Module::property_types, which returns the
     *  full set of property_types that the module can be run as. The final list
     *  may differ from the list returned by this function if the user
     *  explicitly whitelists additional property types.
     *
     *  @return The list of property types that this module can be run as.
     *
     *  @throw none No throw guarantee.
     */
    const auto& property_types() const noexcept { return m_property_types_; }

    /** @brief Returns the RTTI of the derived class.
     *
     *  The type of the derived class is used to index caches. More
     *  specifically, caches are shared by all module instances of the same C++
     *  type. This function is thus used to associate your module with a cache.
     *
     *  @return The RTTI of the derived class.
     *
     *  @throw none No throw guarantee.
     */
    type::rtti type() const noexcept { return m_type_; }

    bool has_uuid() const noexcept { return m_uuid_ != uuid_type{}; }

    uuid_type uuid() const noexcept { return m_uuid_; }

    void set_uuid(uuid_type uuid) noexcept { m_uuid_ = std::move(uuid); }

    /** @brief Returns the documentation on what the derived module does
     *
     *  Developers are encouraged to set a human-readable documentation string
     *  for their modules. If set, that string can be retrieved via this
     *  function.
     *
     *  @note The name of this function is designed to avoid colliding with the
     *        setter.
     *
     *  @return The human-readable documentation of what this module does.
     *
     *  @throw std::bad_optional_access if the description has not been set.
     *                                  Strong throw guarantee.
     */
    const type::description& get_desc() const { return m_desc_.value(); }

    /** @brief Returns the citations
     *
     *  If the developer has added any literature references for the module
     *  those references can be retrieved via this function.
     *
     *  @return An std::vector of the citations assoicated with this module.
     *
     *  @throw none No throw guarantee.
     */
    const auto& citations() const noexcept { return m_citations_; }

    /** @brief Sets where the module can store internal data.
     *
     *  @param[in] cache A shared_ptr to a @p cache_type instance which will be
     *                   used by the module for cacheing internal data.
     *
     *  @throw None No throw guarantee.
     */
    void set_cache(cache_ptr cache) noexcept { m_cache_ = cache; }

    /** @brief Provides the module a place to store internal quantities.
     *
     *  Modules may have need of cacheing intermediate quantities which are not
     *  the main results of the module. Cacheing of the main results is handled
     *  automatically by the pluginplay. Module developers are responsible for
     * cacheing intermediate quantities. This function can be used to retrieve
     * the place where module developers should store these intermediate
     * results.
     *
     *  @return The Cache object that the module should use to for cacheing
     *          internal quantities.
     *
     *  @throw std::runtime_error if there is no cache. Strong throw guarantee.
     */
    cache_type& get_cache() const;

    /** @brief Reset the modules internal cache.
     */
    void reset_internal_cache() const;

    /** @brief Sets the runtime of the module.
     *
     *  @param[in] runtime A shared_ptr to a @p runtime_type instance with which
     *                     the module is currently associated.
     *
     *  @throw None No throw guarantee.
     */
    void set_runtime(runtime_ptr runtime) noexcept { m_runtime_ = runtime; }

    /** @brief Provides the runtime of the module.
     *
     *  @return The runtime object with which the module is currently
     *          associated.
     *
     *  @throw std::runtime_error if there is no runtime. Strong throw
     * guarantee.
     */
    runtime_type& get_runtime() const;

    /** @brief Compares two ModuleBase instances for equality.
     *
     *  Two ModuleBase instances are equivalent if their algorithm is
     *  implemented by the same derived class. This is because that class must
     *  set the inputs, results, submodules, and property_types the same way
     *  every time.
     *
     *  @param[in] rhs The instance to compare against.
     *
     *  @return true if the instances are equal and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool operator==(const ModuleBase& rhs) const noexcept;

    /** @brief Determines if two ModuleBase instances are different
     *
     *  Two ModuleBase instances are equivalent if their algorithm is
     *  implemented by the same derived class. This is because that class must
     *  set the inputs, results, submodules, and property_types the same way
     *  every time.
     *
     *  @param[in] rhs The instance to compare against.
     *
     *  @return false if the instances are equal and true otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool operator!=(const ModuleBase& rhs) const noexcept;

    template<typename InputContainer, typename ResultContainer>
    void satisfies_property_type(type::rtti rtti, InputContainer&& inputs,
                                 ResultContainer&& results) {
        m_property_types_.insert(std::move(rtti));
        m_inputs_.insert(inputs.begin(), inputs.end());
        m_results_.insert(results.begin(), results.end());
    }

protected:
    /// The constructor Python modules must call (second argument is ignored and
    /// is just there so we don't have to rely on SFINAE)
    ModuleBase(const python::PythonWrapper& o, bool);

    /** @brief the ctor that all C++ modules must call.
     *
     *  Classes that derive form ModuleBase need to call this constructor to
     *  initialize the base. They do this by passing their `this` pointer to
     *  this ctor.
     *
     * @tparam T The type of the module deriving from this class.
     *
     * @param[in] ptr the this pointer of the derived instance
     *
     * @throw none No throw guarantee.
     */
    template<typename DerivedType>
    explicit ModuleBase(DerivedType* ptr) noexcept;

    /** @brief Sets the human-readable description of what this module does.
     *
     *  Developers should call this function in their module's ctor to define
     *  what the module does. The value provided to the function will be used
     *  for help and documentation purposes and may include reST. Multiple calls
     *  to this function will overwrite the description.
     *
     *  @param[in] value The human-readable description of this module
     *
     * @throw none No throw guarantee.
     */
    void description(type::description value) noexcept;

    /** @brief Appends a citation
     *
     *  Module developers should call this function to add a citation to their
     *  module. Each call to this function appends a citation onto the internal
     *  list.
     *
     * @param[in] value The new citation.
     *
     * @throw std::bad_alloc if the internal std::vector has insufficient memory
     *                       to store the new citation.
     */
    void citation(type::description value);

    /** @brief Adds an additional input field to this module.
     *
     *  A module automatically has all of the input fields associated with any
     *  property types it satisfies. This function can be called to add
     *  additional input fields to the module. Any input fields added this way
     *  will need to have a default value or be manually set by the user before
     *  the module can be run.
     *
     *  @param T The type of this input field's values.
     *
     *  @param[in] key The name of the input.
     *
     *  @return The newly created input so that it can be modified.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the new
     *                        input field. Strong throw guarantee.
     */
    template<typename T>
    auto& add_input(const type::key& key);

    /** @brief Allows a developer to change an input field
     *
     *  A module automatically has all of the input fields associated with any
     *  property types it satisfies. Sometimes the descriptions and/or defaults
     *  set by those property types are not appropriate. This function returns
     *  an existing input field so that the developer can modify it.
     *
     *  @param[in] key The name of the input field you want to modify.
     *
     *  @return The requested input field
     *
     *  @throw std::out_of_range if @p key is not currently associated with an
     *                           existing input field name. Strong throw
     *                           guarantee.
     */
    auto& change_input(const type::key& key) { return m_inputs_.at(key); }

    /** @brief Adds an additional result field to this module.
     *
     *  A module automatically has all of the result fields associated with any
     *  property types it satisfies. This function can be called to add
     *  additional result fields to the module. Any result fields added this way
     *  will need to be manually requested by the user.
     *
     *  @param T The type of this result field's value.
     *
     *  @param[in] key The name of the result.
     *
     *  @return The newly created result so that it can be modified.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the new
     *                        result field. Strong throw guarantee.
     */
    template<typename T>
    auto& add_result(const type::key& key);

    /** @brief Allows a developer to change a result field
     *
     *  A module automatically has all of the result fields associated with any
     *  property types it satisfies. Sometimes the descriptions and/or defaults
     *  set by those property types are not appropriate. This function returns
     *  an existing result field so that the developer can modify it.
     *
     *  @param[in] key The name of the result field you want to modify.
     *
     *  @return The requested result field
     *
     *  @throw std::out_of_range if @p key is not currently associated with an
     *                           existing result field name. Strong throw
     *                           guarantee.
     */
    auto& change_result(const type::key& key) { return m_results_.at(key); }

    /** @brief Adds an additional submodule request to this module.
     *
     *  A module automatically has all of the submodule requests associated with
     *  any property types it satisfies. This function can be called to add
     *  additional submodule requests to the module. Any requests added this way
     *  will be automatically set to the default module implementation by the
     *  ModuleManager unless the user or developer binds a value to the request.
     *
     *  @param T The property type for the submodule being requested.
     *
     *  @param[in] key The name of the request.
     *
     *  @return The newly created request so it can be modified.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the new
     *                        request. Strong throw guarantee.
     */
    template<typename T>
    auto& add_submodule(const type::key& key);

    /** @brief Allows a developer to change a submodule request
     *
     *  A module automatically has all of the submodule requests associated with
     *  any property types it satisfies. Sometimes the descriptions and/or
     *  defaults set by those property types are not appropriate. This function
     *  returns an existing submodule request so that the developer can modify
     *  it.
     *
     *  @param[in] key The name of the submodule request you want to modify.
     *
     *  @return The requested submodule request
     *
     *  @throw std::out_of_range if @p key is not currently associated with an
     *                           existing submodule request. Strong throw
     *                           guarantee.
     */
    auto& change_submod(const type::key& key) { return m_submods_.at(key); }

    /** @brief Specifies that the derived module satisfies @p property_type
     *
     *  Property types are essentially base classes for modules. This function
     *  allows you to specify that your module "derives" from property type
     *  @p property_type. This will cause the pluginplay to automatically add
     * all inputs and results associated with that property type to the inputs
     *  and results of this module. Additionally, this allows your module to
     *  be used anywhere a module of property type @p property_type is needed.
     *
     *  @tparam property_type
     *
     *  @throw std::bad_alloc if there is insufficient memory to add the
     *                        property type's inputs and results. Weak throw
     *                        guarantee.
     */
    template<typename property_type>
    void satisfies_property_type();

    /** @brief Developer facing API for running the module.
     *
     * This is the member function that the derived class should implement for
     * their module's algorithm. By time this function has been called the
     * inputs to the module have been validated, submodules have been assigned
     * and validated, and memoization has occurred (which means we do not have
     * the result in the cache if this function is being called). The
     * implementation is responsible for using the provided inputs and
     * submodules to compute the results.
     *
     * @param[in] inputs The values your module should use as input
     * @param[in] submods The submodules your module should use during the run.
     *
     * @return Whatever properties your module agreed to compute.
     *
     * @throw ??? Throws if the module throws. Same guarantee.
     */
    virtual type::result_map run_(type::input_map inputs,
                                  type::submodule_map submods) const = 0;

private:
    /// The UUID assigned to this module
    uuid_type m_uuid_;

    /// The inputs set by the developer
    type::input_map m_inputs_;

    /// The results set by the developer
    type::result_map m_results_;

    /// The submodules set by the developer
    type::submodule_map m_submods_;

    /// The property types this module satisfies
    std::set<type::rtti> m_property_types_;

    /// The RTTI of the derived class
    type::rtti m_type_;

    /// What this module does
    std::optional<type::description> m_desc_;

    /// A list of literature citations to cite if you use this module
    std::vector<type::description> m_citations_;

    /// Where the module can store temporaries and intermediates
    cache_ptr m_cache_;

    /// Pointer to this modules current runtime
    runtime_ptr m_runtime_;

    /// Is this module implemented in Python?
    bool m_is_python_ = false;
}; // class ModuleBase

// -------------------------------- Helper Macros ------------------------------

/** @brief Declares a new module.
 *
 *  Modules are implemented by inheriting from ModuleBase and by implementing
 *  a default constructor and a run_ member. This macro will take care of
 *  declaring the derived class for you. The result of using this macro is a
 *  class @p module_name which properly inherits from ModuleBase.
 *
 *  @param[in] module_name The name of the class which will hold your module.
 */
#define DECLARE_MODULE(module_name)                                \
    struct module_name : pluginplay::ModuleBase {                  \
        module_name();                                             \
                                                                   \
    private:                                                       \
        pluginplay::type::result_map run_(                         \
          pluginplay::type::input_map inputs,                      \
          pluginplay::type::submodule_map submods) const override; \
    }

/** @brief Starts the definition of a Module's default constructor.
 *
 *  The module's default constructor establishes what property types it
 *  satisfies and associates meta-data with the module. This macro takes care of
 *  declaring the hook's signature so that all you need to do is declare the
 *  constructor's body.
 *
 *  @param[in] module_name The name of the class defining the module.
 */
#define MODULE_CTOR(module_name) \
    module_name::module_name() : pluginplay::ModuleBase(this)

#define TEMPLATED_MODULE_CTOR(module_name, ...) \
    module_name<__VA_ARGS__>::module_name() : pluginplay::ModuleBase(this)

/** @brief Starts the definition of a module's run hook.
 *
 *  The run hook of a class derived from ModuleBase defines what a module does
 *  when it is executed. The hook is implemented as a member function of the
 *  @p module_name class with the macro taking care of declaring the hook's
 *  signature so that all you need to do is declare the function's body.
 *
 *  @param[in] module_name The name of the class implementing the module.
 *  @param[in] inputs Name given to the first input parameter of the run_
 *                    function. In your function's body use @p inputs to access
 *                    the inputs the user provided to your module.
 *  @param[in] submods Name given to the second input parameter of the run_
 *                     function. In your function's body use @p submods to
 *                     access the submodules provided to your module.
 */
#define MODULE_RUN(module_name)                     \
    pluginplay::type::result_map module_name::run_( \
      pluginplay::type::input_map inputs,           \
      pluginplay::type::submodule_map submods) const

#define TEMPLATED_MODULE_RUN(module_name, ...)                   \
    pluginplay::type::result_map module_name<__VA_ARGS__>::run_( \
      pluginplay::type::input_map inputs,                        \
      pluginplay::type::submodule_map submods) const

//--------------------------------Implementations-------------------------------

inline type::result_map ModuleBase::run(type::input_map inputs,
                                        type::submodule_map submods) const {
    return run_(std::move(inputs), std::move(submods));
}

inline bool ModuleBase::operator==(const ModuleBase& rhs) const noexcept {
    if(type() != rhs.type()) return false;
    if(has_description() != rhs.has_description()) return false;
    if(has_description() && get_desc() != rhs.get_desc()) return false;
    if(m_citations_ != rhs.m_citations_) return false;
    if(m_is_python_ != rhs.m_is_python_) return false;
    return std::tie(inputs(), results(), submods(), property_types()) ==
           std::tie(rhs.inputs(), rhs.results(), rhs.submods(),
                    rhs.property_types());
}

inline bool ModuleBase::operator!=(const ModuleBase& rhs) const noexcept {
    return !((*this) == rhs);
}

template<typename DerivedType>
ModuleBase::ModuleBase(DerivedType*) noexcept :
  m_type_(type::rtti(typeid(std::decay_t<DerivedType>))) {
    using clean_t          = std::decay_t<DerivedType>;
    constexpr bool is_base = std::is_base_of_v<ModuleBase, clean_t>;
    static_assert(
      is_base, "The type of the input is not a class dervied from ModuleBase");
}

inline ModuleBase::ModuleBase(const python::PythonWrapper& self, bool) :
  m_type_(type::rtti(typeid(python::PythonWrapper))), m_is_python_(true) {}

inline void ModuleBase::description(type::description value) noexcept {
    m_desc_.emplace(std::move(value));
}

inline void ModuleBase::citation(type::description value) {
    m_citations_.push_back(std::move(value));
}

template<typename T>
auto& ModuleBase::add_input(const type::key& key) {
    return m_inputs_[key].set_type<T>();
}

template<typename T>
auto& ModuleBase::add_result(const type::key& key) {
    return m_results_[key].set_type<T>();
}

template<typename T>
auto& ModuleBase::add_submodule(const type::key& key) {
    return m_submods_[key].set_type<T>();
}

template<typename property_type>
void ModuleBase::satisfies_property_type() {
    property_type p;
    type::rtti rtti(typeid(property_type));
    satisfies_property_type(rtti, p.inputs(), p.results());
}

inline typename ModuleBase::cache_type& ModuleBase::get_cache() const {
    if(!m_cache_)
        throw std::runtime_error("Module does not have an interal cache");
    return *m_cache_.get();
}

inline typename ModuleBase::runtime_type& ModuleBase::get_runtime() const {
    if(!m_runtime_)
        throw std::runtime_error("Module does not a Runtime instance");
    return *m_runtime_.get();
}

inline void ModuleBase::reset_internal_cache() const {
    if(m_cache_) m_cache_->reset_cache();
}

} // namespace pluginplay
