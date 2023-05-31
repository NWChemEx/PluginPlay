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
#include "pluginplay/module.hpp"
#include "pluginplay/types.hpp"
#include <memory>
#include <string>

namespace pluginplay {
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

    /// Type used to hold the wrapped Module's UUID
    using uuid_type = typename Module::uuid_type;

    /// Type of map, mapping submodule keys to UUIDs
    using submod_uuid_map = typename Module::submod_uuid_map;

    /// Type used for RTTI
    using rtti_type = std::type_index;

    /** @brief Makes an empty request.
     *
     *  The request resulting from this call will have no description, no type,
     *  and no module.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        PIMPL. Strong throw guarantee.
     */
    SubmoduleRequest();

    /** @brief Sets the current request's state to a deep copy of @p rhs
     *
     *  This funciton will make a deep copy of another SubmoduleRequest's
     *  state. This includes any bound module, if it exists.
     *
     *  @param[in] rhs The instance's state to copy.
     *
     *  @throw std::bad_alloc if there is insufficient memory for the copy.
     *                        Strong throw guarantee.
     */
    SubmoduleRequest(const SubmoduleRequest& rhs);

    /** @brief Sets the current request's state to a deep copy of @p rhs
     *
     *  This function will overwrite the current instance's state with a deep
     *  copy of another SubmoduleRequest's state. This includes any bound
     *  module, if it exists.
     *
     *  @param[in] rhs The instance's state to copy.
     *
     *  @return The current instance containing a deep copy of @p rhs's state.
     *
     *  @throw std::bad_alloc if there is insufficient memory for the copy.
     *                        Strong throw guarantee.
     */
    SubmoduleRequest& operator=(const SubmoduleRequest& rhs);

    /** @brief Takes ownership of another instance's state.
     *
     *  This ctor will make the current instance own @p rhs's state.
     *
     *  @param[in] rhs The instance whose state will be taken. After this
     *                 operation @p rhs is in a valid, but otherwise undefined
     *                 state.
     *
     *  @throw none No throw guarantee.
     */
    SubmoduleRequest(SubmoduleRequest&& rhs) noexcept;

    /** @brief Takes ownership of another instance's state.
     *
     *  This function will overwrite the current instance's state with @p rhs's
     *  state.
     *
     *  @param[in] rhs The instance whose state will be taken. After this
     *                 operation @p rhs is in a valid, but otherwise undefined
     *                 state.
     *
     *  @return The current instance with @p rhs's state.
     *
     *  @throw none No throw guarantee.
     */
    SubmoduleRequest& operator=(SubmoduleRequest&& rhs) noexcept;

    /** @brief Standard destructor
     *
     *  After calling the dtor all references to metadata are invalid and the
     *  reference count of the wrapped module is decreased by one. Hence
     *  references to the module remain valid after the call to the dtor, until
     *  the reference count goes to zero.
     *
     *  @throw none No throw guarantee.
     */
    ~SubmoduleRequest() noexcept;

    /** @brief Has the property type the submodule needs to satisfy been set?
     *
     *  This request is for a submodule that will compute a property for the
     *  parent module. This function is used to determine if the property type
     *  that the submodule must satisfy has been set.
     *
     *  @return  True if the property type has been set and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_type() const noexcept;

    /** @brief Has this request been fulfilled?
     *
     *  This function checks to see if a submodule has been assigned to this
     *  request. Use set_module to actually fulfill the request.
     *
     *  @return True if this request is satisfied and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_module() const noexcept;

    /** @brief Has the description been set?
     *
     *  Developers are encouraged to explain how a submodule will be used. This
     *  prose is the description of the SubmoduleRequest and is used for
     *  documentation and help purposes only. This function checks if the
     *  description has been set.
     *
     *  @return True if the description has been set and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_description() const noexcept;

    /** @brief Determines if the wrapped submodule can be used
     *
     * A submodule can be used if there is actually a submodule wrapped in the
     * request and if that submodule reports that it is ready for use (the
     * criteria for it being ready for use are identical to the Module class).
     *
     * @return True if the current request contains a submodule that is ready
     *         for use and false otherwise.
     *
     * @throw none No throw guarantee.
     *
     */
    bool ready() const noexcept;

    /** @brief Sets the property type the submodule must satisfy
     *
     *
     *  @tparam T The type of the property type
     *
     *  @return The current instance with the required property type set to
     *          @p T.
     *
     *  @throw std::runtime_error if the module has already been set and the
     *                            requested property type is inconsistent with
     *                            the module. Strong throw guarantee.
     */
    template<typename T>
    auto& set_type();

    /** @brief Uses RTTI to set the type of *this.
     *
     *  The templated `set_type` method is the preferred means of setting the
     *  property type from C++. If we want to set the property type from Python
     *  we need to set it at runtime. This overload of set_type allows the
     *  property type to be set at runtime.
     */
    SubmoduleRequest& set_type(rtti_type type, type::input_map inputs);

    /** @brief Sets the module that is to be used to satisfy the request
     *
     *  This function is designed to be called by the ModuleManager via the
     *  SubmoduleRequest class in order to fulfill the request.
     *
     *  @param[in] ptr Pointer to the module to use.
     *
     *  @throw std::runtime_error if @p ptr is null. Strong throw guarantee.
     *  @throw std::bad_optional_access if the property type has not been set
     *                                  yet. Strong throw guarantee.
     *  @throw std::runtime_error if @p ptr is not of the correct property type.
     *                            Strong throw guarantee.
     */
    void change(module_ptr new_module);

    /** @brief Sets the human-readable description for the this request.
     *
     *  Developers are encouraged to provide descriptions for how a submodule
     *  will be used. This description is set by calling this function. If the
     *  request contains a description prior to this call it will be
     *  overwritten by this call.
     *
     *  @param[in] desc The new value for this request's description.
     *
     *  @return The current instance with the description set to @p desc
     *
     *  @throw none No throw guarantee.
     */
    SubmoduleRequest& set_description(type::description desc) noexcept;

    /** @brief Get the RTTI of the property type this submodule must satisfy
     *
     *  The developer must specify the property type that the submodule
     *  satisfies using set_type. This function is the accessor providing
     *  access to the type that the developer specified.
     *
     *  @return The RTTI of the property type that the submodule must satisfy.
     *
     *  @throw std::bad_optional_access if has_type() is false. Strong throw
     *                                  guarantee.
     */
    type::rtti type() const;

    /** @brief This is a convenience function for calling submod_uuids on the
     *         wrapped module.
     *
     *  For memoization purposes we need to know the UUIDs of each submodule.
     *  Retrieving this information is done by calling Module::submod_uuids().
     *  This function is a convenience function so that users don't need to
     *  unwrap the module in the SubmoduleRequest to call submod_uuids() (this
     *  function does that for you).
     *
     *  @return A map from submodule keys to UUIDs.
     *
     *  @throw std::bad_alloc if there is a problem making the map. Strong
     *                        throw guarantee.
     *  @throw std::runtime_error if there is no wrapped module. Strong throw
     *                            guarantee.
     */
    submod_uuid_map submod_uuids() const;

    /** @brief Returns the UUID of the wrapped module.
     *
     *  @throw std::runtime_error if there is no wrapped module. Strong throw
     *                            guarantee.
     */
    uuid_type uuid() const;

    /** @brief Provides the module used to satisfy this request in a read-only
     *         state.
     *
     *  This function allows one to retrieve the module used to satisfy the
     *  request in a read-only state. This is useful for inspection and
     *  running the module.
     *
     *  @return The module satisfying this request.
     *
     *  @throw std::runtime_error if the request has not been fulfilled yet.
     *                            Strong throw guarantee.
     */
    const Module& value() const;

    /** @brief Provides the module used to satisfy this request in a read/write
     *         state.
     *
     *  This function allows one to retrieve the module used to satisfy the
     *  request in a read/write state. This is useful for changing inputs before
     *  running the module.
     *
     *  @return The module satisfying this request.
     *
     *  @throw std::runtime_error if the request has not been fulfilled yet.
     *                            Strong throw guarantee.
     */
    Module& value();

    /** @brief Used to retrieve the description of this request.
     *
     *  Developers are encouraged to set the description of how a submodule will
     *  be used. If they set this description it can be retrieved via this
     *  function.
     *
     * @return The human-readable description of how this submodule will be used
     *
     * @throw std::bad_optional_access if the description has not been set yet.
     *                                 Strong throw guarantee.
     */
    const type::description& description() const;

    /** @brief Locks the submodule.
     *
     *  A locked module can no longer have its inputs and submodules modified.
     *  Locking a module helps avoid data races and problems with memoization
     *  that can occur if the inputs/submodules changed. This function locks the
     *  submodule. Since it would not be possible to ready a locked submodule
     *  (recall the property type is known and set) this function will throw if
     *  one attempts to lock an unready module.
     *
     *
     *  @throw std::runtime_error if the submodule is not ready. Strong throw
     *                            guarantee.
     */
    void lock();

    /** @brief Runs the submodule with the given arguments
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
     *
     * @param[in] args The values for the arguments to forward to the submodule.
     *
     *
     * @return whatever the property type returns
     *
     * @throw std::bad_optional_access if the type has not been set yet. Strong
     *                                 throw guarantee.
     * @throw std::invalid_argument if the submodule is being run as a property
     *                             type other than the one it should be. Strong
     *                             throw guarantee.
     * @throw std::runtime_error if the submodule has not been set yet. Strong
     *                             throw guarantee.
     */
    template<typename property_type, typename... Args>
    auto run_as(Args&&... args);

    /** @brief Compares two SubmoduleRequest instances for equality
     *
     * Two SubmoduleRequest instances are equivalent if they both:
     * - request the same property type,
     * - have the same description, and
     * - are not satisfied or are both satisfied with the same module
     *
     * @param[in] rhs The request to compare against
     *
     * @return True if the this instance is the same as @p rhs and false
     *              otherwise.
     *
     * @throw ??? If the comparison between the two modules throws. Same throw
     *        guarantee.
     */
    bool operator==(const SubmoduleRequest& rhs) const;

    /** @brief Dertermines if two SubmoduleRequest instances are different
     *
     * Two SubmoduleRequest instances are equivalent if they both:
     * - request the same property type,
     * - have the same description, and
     * - are not satisfied or are both satisfied with the same module
     *
     * @param[in] rhs The request to compare against
     *
     * @return False if the this instances are the same and true otherwise.
     *
     * @throw ??? If the comparison between the two modules throws. Same throw
     *        guarantee.
     */
    bool operator!=(const SubmoduleRequest& rhs) const;

private:
    /// Object actually storing the state of this class
    std::unique_ptr<detail_::SubmoduleRequestPIMPL> m_pimpl_;
};

//----------------------Implementations-----------------------------------------

inline SubmoduleRequest& SubmoduleRequest::operator=(
  const SubmoduleRequest& rhs) {
    return *this = std::move(SubmoduleRequest(rhs));
}

template<typename T>
auto& SubmoduleRequest::set_type() {
    using clean_t = std::decay_t<T>;
    auto inps     = clean_t::inputs();
    type::input_map inputs(inps.begin(), inps.end());
    return set_type(rtti_type(typeid(clean_t)), std::move(inputs));
}

template<typename property_type, typename... Args>
auto SubmoduleRequest::run_as(Args&&... args) {
    if(type() != std::type_index(typeid(property_type)))
        throw std::invalid_argument("Wrong property type");
    return value().run_as<property_type>(std::forward<Args>(args)...);
}

inline bool SubmoduleRequest::operator!=(const SubmoduleRequest& rhs) const {
    return !((*this) == rhs);
}

} // namespace pluginplay
