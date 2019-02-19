#pragma once
#include "SDE/Module.hpp"
#include "SDE/Types.hpp"
#include <memory>
#include <string>
#include <typeindex>

namespace SDE {
template<typename T>
class PropertyType;
namespace detail_ {
class SubmoduleRequestPIMPL;
}

/** @brief Class that wraps a module's request for a particular submodule.
 *
 *  When a module needs to use a submodule as part of its `run` function, the
 *  module creates an instance of this class. For the most part, a submodule
 *  request comes down to one thing, the property type that the submodule
 *  must satisfy. The SubmoduleRequest class stores the requested property type
 *  as well as some other metadata such as the description.
 *
 *  Note that there is a cyclic dependency between the SubmoduleRequest class
 *  and the Module class, which results from the fact that a Module holds a
 *  list of submodule requests and that each submodule request holds the module
 *  that is being used to satisfy the request. In practice this dependency is
 *  trivially handled because both classes only need forward declarations of
 *  the other for their respective declarations.
 *
 */
class SubmoduleRequest {
public:
    /// Type of a shared pointer to a module, how the submodule is stored
    using module_ptr = std::shared_ptr<Module>;

    ///@{
    /** @name Ctor and assignment operators
     *
     * Functions in this section create a SubmoduleRequest either in a default
     * state or a state copied/taken from another instance. Customization of the
     * state is accomplished using the setters. All copies are deep copies.
     *
     * @param rhs The other instance to copy/move from. If moving from, @p rhs
     *        is in a valid, but otherwise undefined state after the operation.
     *
     * @throw std::bad_alloc if there is insufficient memory to make a new
     *        pimpl. 2 and 3 additionally throw if there is insufficient memory
     *        to copy the underlying module.
     * @throw none 4 and 5 are no throw guarantee.
     */
    SubmoduleRequest();
    SubmoduleRequest(const SubmoduleRequest& rhs);
    SubmoduleRequest& operator=(const SubmoduleRequest& rhs) {
        return *this = std::move(SubmoduleRequest(rhs));
    }
    SubmoduleRequest(SubmoduleRequest&& rhs) noexcept;
    SubmoduleRequest& operator=(SubmoduleRequest&& rhs) noexcept;
    ///@}

    /** @brief Standard destructor
     *
     * After calling the dtor all references to metadata are invalid and the
     * reference count of the wrapped module is decreased by one. Hence
     * references to the module remain valid after the call to the dtor, until
     * the reference count goes to zero.
     *
     * @throw none No throw guarantee.
     */
    ~SubmoduleRequest() noexcept;

    /** @brief Runs the
     *
     * This function is a convenience function for running the wrapped submodule
     * as a particular property type. It is semantically the same as calling:
     *
     * ```
     * this->value().run_as<T>(args...);
     * ```
     *
     * aside from the fact that it also asserts that the submodule is being run
     * as the correct property type. Expert users can access the wrapped module
     * via the `value` member and perform more advanced runs that way.
     *
     * @tparam property_type The class defining the property type that the
     *         submodule should be run as.
     * @tparam Args The types of the arguments to the property type
     * @param args The values of the arguments corresponding to the property
     *        type.
     * @return whatever the property type returns
     * @throw std::invalid_argument if the submodule is being run as a property
     *        type other than the one it should be. Strong throw guarantee.
     */
    template<typename property_type, typename... Args>
    auto run_as(Args&&... args) {
        if(!check_type_(typeid(property_type)))
            throw std::invalid_argument("Wrong property type");
        return value().run_as<property_type>(std::forward<Args>(args)...);
    }

    ///@{
    /** @name Getters
     *
     * Functions in this section allow inquiries into the state of the current
     * instance.
     *
     * Respectively the piece of accessed state is:
     * - the wrapped module
     * - the description of the module
     *
     * @return the requested piece of state.
     */
    const Module& value() const;
    Module& value() {
        const auto& rv = const_cast<const SubmoduleRequest&>(*this).value();
        return const_cast<Module&>(rv);
    }
    const type::description& description() const noexcept;
    const std::type_index& type() const;
    ///@}

    ///@{
    /** @name Setters
     *
     * Functions in this section can be used to change the state of the
     * instance.
     *
     * Respectively they change:
     *
     * - the wrapped module
     * - the property type the wrapped module must satisfy
     * - the description of how the wrapped module will be used
     *
     * @param new_module The module to change to.
     * @param desc The new description of the request
     * @return 2 and 3 return the current instance, modified accordingly to
     *         support chaining.
     * @throw std::runtime_error thrown by 2 if the type is already set. Strong
     *        throw guarantee.
     */
    void change(module_ptr new_module) noexcept;

    template<typename T>
    auto& set_type() {
        using clean_t = std::decay_t<T>;
        set_type_(typeid(clean_t));
        return *this;
    }

    SubmoduleRequest& set_description(type::description desc) noexcept;
    ///@}

    /** @brief Hashes the module wrapped in the class.
     *
     *  @param h The hasher instance to use.
     */
    void hash(type::hasher& h) const;

    /** @brief Determines if the wrapped submodule can be used
     *
     * A submodule can be used if there is actually a submodule wrapped in the
     * request and if that submodule reports that it is ready for use (the
     * criteria for it being ready for use are identical to the Module class).
     *
     * @return True if the current request contains a submodule that is ready
     *         for use and false otherwise.
     * @throw none No throw gurantee.
     *
     */
    bool ready() const noexcept;

    /** @brief Locks the wrapped submodule
     *
     *  Locking a submodule has the same effect as locking a Module, namely the
     *  inputs and submodules bound to the instance can not be changed any
     *  longer.
     *
     *  @throws std::runtime_error if the wrapped submodule is not ready to be
     *          locked (*i.e.*, if `ready` returns false). Strong throw
     *          guarantee.
     */
    void lock();

    ///@{
    /** @name Equality comparisons
     *
     *  Two SubmoduleRequests are equivalent if the wrapped module compares
     *  equal (either because both wrap a nullptr or because the actual
     *  wrapped instances are equal), both have the same description, and the
     *  required property type is the same.
     *
     *  @param rhs The instance to compare against.
     *  @return True if the comparison is true and false otherwise
     *
     *  @throw ??? If the wrapped modules throw. Same guarantee as the modules.
     */
    bool operator==(const SubmoduleRequest& rhs) const;
    bool operator!=(const SubmoduleRequest& rhs) const {
        return !((*this) == rhs);
    }
    ///@}
private:
    ///@{
    /** @name Bridging functions
     *
     *  Functions in this section bridge the templated public API functions to
     *  the corresponding backend implementation.
     *
     *  @param type The property type that the submodule must satisfy.
     *  @return 1 returns true if the provided type is the same as the property
     *          type the submodule must have and false otherwise.
     */
    bool check_type_(const std::type_info& type) const noexcept;
    void set_type_(const std::type_info& type);
    ///@}

    /// Object actually storing the state of this class
    std::unique_ptr<detail_::SubmoduleRequestPIMPL> pimpl_;
};

} // namespace SDE
