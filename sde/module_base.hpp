#pragma once
#include "sde/module_input.hpp"
#include "sde/module_result.hpp"
#include "sde/submodule_request.hpp"
#include <utilities/containers/case_insensitive_map.hpp>

namespace sde {

/** @brief API determining how the SDE interacts with a module's implementation
 *
 *  Module implementations derive from the ModuleBase class. The class itself is
 *  used as a PIMPL for the Module class.
 */
class ModuleBase {
public:
    /// The type returned by memoization
    using hash_type = std::string;

    //@{
    /** @name Ctors and Assignment Operators
     *
     *  ModuleBase is meant to be inherited from. In order to avoid the user
     *  having to perform polymorphic copies/moves correctly, we simply disable
     *  copying/moving ModuleBase objects. In order to portray standard C++
     *  object behavior to the outside world the state of the module is copied
     *  into the ModulePIMPL class and copies/moves deal with that state. This
     *  also has the advantage of preserving the developer's original settings
     *  while still allowing the user to set their own defaults. The default
     *  ctor is deleted to avoid developers erroneously forgetting to set the
     *  type (this is done implicitly by passing the this pointer).
     */
    ModuleBase()                      = delete;
    ModuleBase(const ModuleBase& rhs) = delete;
    ModuleBase& operator=(const ModuleBase& rhs) = delete;
    ModuleBase(ModuleBase&& rhs)                 = delete;
    ModuleBase& operator=(ModuleBase&& rhs) = delete;
    //@}

    /** @brief Standard dtor
     *
     *  After calling this function all references to the state are invalidated.
     *  Of note, however, is that results will only be deleted if the reference
     *  count within the enclosing `shared_ptr` instance goes to zero.
     *
     *  @throw none No throw guarantee.
     */
    virtual ~ModuleBase() = default;

    /** @brief SDE-facing API for running the module.
     *
     * This function is used by the SDE to run the module. Values provided to
     * it have already been validated, and memoization has been attempted.
     *
     * @param inputs The values the module should use as input.
     * @param submods The values the submodules the module should use
     * @return The result
     */
    type::result_map run(type::input_map inputs,
                         type::submodule_map submods) const {
        return run_(std::move(inputs), std::move(submods));
    }

    ///@{
    /** @name State accessors
     *
     * Functions in this section provide read-only access to the state of the
     * module as set by the developer of the module. Respectively those pieces
     * of information are the:
     *
     * - list of properties the module computes
     * - list of inputs the module accepts
     * - list of modules the module calls
     * - RTTI for the class that implements the actual algorithm
     *
     * @return The requested piece of state.
     * @throw none All functions are no throw guarantee.
     */
    const type::result_map& results() const noexcept { return results_; }
    const type::input_map& inputs() const noexcept { return inputs_; }
    const type::submodule_map& submods() const noexcept { return submods_; }
    const std::type_info& type() const noexcept { return type_(); }
    const auto& property_types() const noexcept { return m_property_types_; }
    ///@}

    ///@{
    /** @name Comparison operators
     *
     *  Two ModuleBase instances are equivalent if their algorithm is
     *  implemented by the same derived class. This is because the only ctor
     *  that is ever invoked is the default ctor and that ctor must set the
     *  state of the module the same everytime.
     *
     *  @param rhs The instance to compare against.
     *  @return true if the comparison is true and false otherwise
     *  @throw none No throw guarantee.
     */
    bool operator==(const ModuleBase& rhs) const noexcept {
        return type() == rhs.type();
    }

    bool operator!=(const ModuleBase& rhs) const noexcept {
        return !((*this) == rhs);
    }
    ///@}
protected:
    /** @brief the Ctor that all modules must call.
     *
     * Classes that derive form ModuleBase need to call this constructor to
     * initialize the base. They do this by passing the `this` pointer to this
     * ctor.
     *
     * @tparam T The type of the module
     * @param[in] ptr the this pointer of the derived type
     * @throw none No throw guarantee.
     */
    template<typename DerivedType>
    explicit ModuleBase(DerivedType* /*ptr*/) :
      m_type_(typeid(std::decay_t<DerivedType>)) {
        using clean_t = std::decay_t<DerivedType>;
        static_assert(
          std::is_base_of_v<ModuleBase, clean_t>,
          "The input pointer does not appear to be the derived class's "
          "this pointer.");
    }

    ///@{
    /** @name Metadata setters
     *
     * The member functions in this section can be used to set the metadata
     * associated with a module. They are protected so that they can be used by
     * the derived class, but not by users. This is because the developer is in
     * a position to set these values and it does not make sense for the user to
     * change what the developer set the values to.
     *
     * @param value The new value of the piece of metadata. For citations the
     *        contents of @p value are appended to a list.
     * @throw std::bad_alloc 2 throws if there is insufficient memory to
     *        expand the vector. Strong throw guarantee.
     * @throw none 1 is no throw guarantee.
     */
    void description(type::description value) noexcept {
        desc_ = std::move(value);
    }

    void citation(type::description value) {
        citations_.push_back(std::move(value));
    }
    ///@}

    ///@{
    /** @name API setters
     *
     * The final API of a module is the union of the APIs for all property types
     * it satisfies, as well as any additional input, results, or submodule
     * values it needs to run. Values that are added that are not part of
     * a property type's API must be manipulated by the user from the top-level
     * (or have default values) in order for the module to work. The developer
     * also has access to the currently set values *via* the `change_xxx`
     * member functions and can refine defaults, descriptions, etc. that were
     * set by a property type.
     *
     * The `satisfies_property_type` is a convenience function that
     * automatically adds the inputs and results from a property type to this
     * module's API.
     *
     * @tparam T For 1, the type of the additional input, for 3 the type
     *           of the new result, for 5 the property type of the new
     *           submodule, and for 7 the property type the class satisfies.
     * @param key For adding a field the keyword to associate with the field.
     *            For modifications the keyword of the field to modify.
     * @return All functions with a return yield the field so that the developer
     *         can further modify it.
     * @throw std::bad_alloc if there is insufficient memory to add the new
     *        values. All functions are strong throw guarantee except for 7,
     *        which is weak throw guarantee.
     * @throw std::out_of_range is thrown by 2, 4, and 6 if the provided key is
     *        not a valid key. Strong throw guarantee.
     *
     */
    template<typename T>
    auto& add_input(type::key key) {
        return inputs_[key].set_type<T>();
    }

    auto& change_input(type::key key) { return inputs_.at(key); }

    template<typename T>
    auto& add_result(type::key key) {
        return results_[key].set_type<T>();
    }

    auto& change_result(type::key key) { return results_.at(key); }

    template<typename T>
    auto& add_submodule(type::key key) {
        return submods_[key].set_type<T>();
    }

    auto& change_submod(type::key key) { return submods_.at(key); }

    template<typename property_type>
    void satisfies_property_type() {
        property_type p;
        m_property_types_.insert(type::rtti(typeid(property_type)));
        auto inputs = p.inputs();
        inputs_.insert(inputs.begin(), inputs.end());
        auto results = p.results();
        results_.insert(results.begin(), results.end());
    }
    ///@}

private:
    /** @brief Developer facing API for running the module.
     *
     * This is the member function that the derived class should implement for
     * their module's algorithm. By time this function has been called the
     * inputs to the module have been validated, submodules have been assigned
     * and validated, and memoization has occurred (which means we do not have
     * the result in the cache if this function is being called).
     *
     * @param inputs The values of the inputs to use during the run call.
     * @param submods The submodules to use during the run call.
     * @return Whatever properties your module computes.
     */
    virtual type::result_map run_(type::input_map inputs,
                                  type::submodule_map submods) const = 0;

    /** @brief Implements the `type` function
     *
     *  Derived classes are responsible for implementing this function. The
     *  easiest way to do this is to inherit from the `ModuleBaseHelper` class,
     *  which will in turn implement it for you.
     *
     *  @return The RTTI of the derived class
     *  @throw none No throw guarantee.
     */
    const std::type_info& type_() const noexcept { return m_type_; }

    //@{
    /** @name Developer set state
     *
     *  The members in this section contain the lists of inputs, results, and
     *  submodules as set-up by the developer. They will be maintained in this
     *  state until the instance is deleted. Modifications to the state of the
     *  module will occur in the ModulePIMPL class; it is ultimately the state
     *  that lives in the ModulePIMPL class that will be provided to the module
     *  when the run member is invoked.
     */
    type::input_map inputs_;
    type::result_map results_;
    type::submodule_map submods_;

    std::set<type::rtti> m_property_types_;
    //@}

    //@{
    /** @name Module metadata
     *
     *  Respectively the metadata includes:
     *
     *  - The RTTI of the derived class
     *  - The description of what the module does
     *  - A list of articles that should be cited
     */
    const std::type_info& m_type_;
    type::description desc_;
    std::vector<type::description> citations_;
    //@}
};

} // namespace sde
