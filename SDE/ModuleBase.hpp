#pragma once
#include "SDE/ModuleInput.hpp"
#include "SDE/ModuleResult.hpp"
#include "SDE/SubmoduleRequest.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>

namespace SDE {

/** @brief API determining how the SDE interacts with a module's implementation
 *
 *  Module implementations derive from the ModuleBase class. The class itself is
 *  used as a PIMPL for the Module class.
 */
class ModuleBase {
public:
    using input_map  = Utilities::CaseInsensitiveMap<ModuleInput>;
    using output_map = Utilities::CaseInsensitiveMap<ModuleResult>;
    using submod_map = Utilities::CaseInsensitiveMap<SubmoduleRequest>;
    using hash_type  = std::string;

    //@{
    /** @name Ctors and Assignment Operators
     *
     *  ModuleBase is meant to be inherited from. In order to avoid the user
     *  having to perform polymorphic copies/moves correctly, we simply disable
     *  copying/moving ModuleBase objects. In order to portray standard C++
     *  object behavior to the outside world the state of the module is copied
     *  into the ModulePIMPL class and copies/moves deal with that state. This
     *  also has the advantage of preserving the developer's original settings
     *  while still allowing the user to set their own defaults.
     */
    ModuleBase()                      = default;
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
    output_map run(input_map inputs, submod_map submods) const {
        return run_(std::move(inputs), std::move(submods));
    }

    /** @brief computes a hash for a particular invocation of the `run` member.
     *
     * For a deterministic module providing the module the same inputs must
     * return the same outputs. We need a way to determine if we have already
     * called the module with a particular set of inputs; that's where this
     * function comes in. This function takes a set of input values, as well as
     * the set of submodules to use, and maps them to a hash value. Barring the
     * universe conspiring against us, that hash value is a concise and unique
     * representation of the input state.
     *
     * @param h The hasher instance to use
     * @param inputs The values of the inputs to hash
     * @param submods The values of the submodules to use.
     *
     */
    void memoize(type::hasher& h, const input_map& inputs,
                 const submod_map& submods) const {
        for(const auto & [k, v] : inputs) v.hash(h);
        for(const auto & [k, v] : submods) v.hash(h);
    }

    ///@{
    /** @name State accessors
     *
     * Functions in this section provide read-only acccess to the state of the
     * module as set by the developer of the module.
     *
     * @return The requested piece of state.
     */
    const output_map& results() const { return results_; }
    const input_map& inputs() const { return inputs_; }
    const submod_map& submods() const { return submods_; }
    ///@}
protected:
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
    auto& add_output(type::key key) {
        return results_[key].set_type<T>();
    }
    auto& change_output(type::key key) { return results_.at(key); }
    template<typename T>
    auto& add_submodule(type::key key) {
        return submods_[key].set_type<T>();
    }
    auto& change_submod(type::key key) { return submods_.at(key); }

    template<typename property_type>
    void satisfies_property_type() {
        property_type p;
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
    virtual output_map run_(input_map inputs, submod_map submods) const = 0;

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
    input_map inputs_;
    output_map results_;
    submod_map submods_;
    //@}

    //@{
    /** @name Module metadata
     *
     *  Respectively the metadata includes:
     *
     *  - The description of what the module does
     *  - A list of articles that should be cited
     */
    type::description desc_;
    std::vector<type::description> citations_;
    //@}
};

} // namespace SDE
