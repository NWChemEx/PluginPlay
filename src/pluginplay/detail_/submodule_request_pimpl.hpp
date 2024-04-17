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
#include "pluginplay/submodule_request.hpp"
#include <optional>
#include <utilities/printing/demangler.hpp>

namespace pluginplay::detail_ {

/** @brief Guts of the SubmoduleRequest class
 *
 *  Modules typically rely on other modules to perform subtasks for them. When a
 *  module X calls a module Y to run a subtask, module Y is said to be a
 *  submodule of module X. The SubmoduleRequest class defines the API for the
 *  developer of module X to request Y. The SubmoduleRequestPIMPL class actually
 *  implements the SubmoduleRequest class.
 *
 *  @par Why Does This Class Have Inputs?
 *
 *  Before a module can be run we need to know that all of its submodules are
 *  ready. This requires us to call ready on each submodule. However, unlike the
 *  call at the top-level, which has the inputs available to it the submodules
 *  do not. Furthermore, forwarding the inputs from the top-level is unlikely
 *  to solve the problem because inputs to the submodule can be computed as
 *  part of the caller. Thankfully we know what property type the submodule will
 *  be run as, and then consequentially we know what types of inputs will be
 *  provided to this submodule when it is called. This is what the `m_inputs_`
 *  member of this class does; it saves a dummy set of inputs to use for the
 *  check.
 */
class SubmoduleRequestPIMPL {
public:
    /// Type used to hold the submodule
    using module_ptr = typename SubmoduleRequest::module_ptr;

    using uuid_type = typename SubmoduleRequest::uuid_type;

    using submod_uuid_map = typename SubmoduleRequest::submod_uuid_map;

    /** @brief Makes an empty request.
     *
     *  The PIMPL resulting from this call will have no description, no type,
     *  and no module.
     *
     *  @throw none No throw guarantee.
     */
    SubmoduleRequestPIMPL() = default;

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
    SubmoduleRequestPIMPL(const SubmoduleRequestPIMPL& rhs) = default;

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
    SubmoduleRequestPIMPL& operator=(const SubmoduleRequestPIMPL& rhs) =
      default;

    /** @brief Takes ownership of another instance's state.
     *
     *  This ctor will make the current instance own @p rhs's state. The state
     *  owned by the resulting instance will not be a copy of @p rhs's state,
     *  but will own the actual state. After this operation @p rhs will no
     *  longer have access to its original state and its state will be in a
     *  valid, but otherwise undefined state.
     *
     *  @param[in] rhs The instance whose state will be taken. After this
     *                 operation @p rhs is in a valid, but otherwise undefined
     *                 state.
     *
     *  @throw none No throw guarantee.
     */
    SubmoduleRequestPIMPL(SubmoduleRequestPIMPL&& rhs) = default;

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
    SubmoduleRequestPIMPL& operator=(SubmoduleRequestPIMPL&& rhs) = default;

    /** @brief Makes a deep copy of the submodule request on the heap.
     *
     *  This function is a convenience function to be used by the
     *  SubmoduleRequest class in order to copy the PIMPL. Should the PIMPL be
     *  made polymorphic then this function would also serve as the polymorphic
     *  copy ctor.
     *
     *  @return A unique pointer to the deep copy.
     *
     *  @throw std::bad_alloc if there is insufficient memory to make the copy.
     *                        Strong throw guarantee.
     */
    auto clone() const;

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
    bool has_type() const noexcept { return m_type_.has_value(); }

    /** @brief Has this request been fulfilled?
     *
     *  This function checks to see if a submodule has been assigned to this
     *  request. Use set_module to actually fulfill the request.
     *
     *  @return True if this request is satisfied and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_module() const noexcept { return static_cast<bool>(m_module_); }

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
    bool has_description() const noexcept { return m_desc_.has_value(); }

    /** @brief Has this request been fulfilled and is that module ready?
     *
     *  This function is slightly different than has_module. The former only
     *  checks if the module is set, whereas this function additionally checks
     *  that the submodule is ready to be run provided it is given inputs of the
     *  types stored in `m_inputs_`.
     *
     *  @return True if the submodule is ready to run and false otherwise.
     *
     *  @throw std::bad_alloc if there is insufficient memory for
     *                        Module::ready() throws. Strong throw guarantee.
     */
    bool ready() const;

    /** @brief Sets the property type the submodule must satisfy
     *
     *
     *
     *  @param[in] type The RTTI of the property type
     *  @param[in] inputs A set of inputs consistent with the property type's
     *                    API.
     *
     * @throw std::runtime_error if the property type has already been set.
     *        Strong throw guarantee.
     */
    void set_type(type::rtti type, type::input_map inputs);

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
    void set_module(module_ptr ptr);

    /** @brief Sets the human-readable description for the this request.
     *
     *  Developers are encouraged to provide descriptions for how a submodule
     *  will be used. This description is set by calling this function. If the
     *  request contains a description prior to this call it will be
     *  overwritten by this call.
     *
     *  @param[in] desc The new value for this request's description.
     *
     *  @throw none No throw guarantee.
     */
    void set_description(std::string desc) noexcept;

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
    type::rtti type() const { return m_type_.value(); }

    /** @brief Provides the module used to satisfy this request.
     *
     *  This function will ultimately be used by SubmoduleRequest::run_as in
     *  order to run the submodule.
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
    const auto& description() const { return m_desc_.value(); }

    /** @brief Convenience function for calling submod_uuids() on the wrapped
     *         module.
     *
     *  This function simply wraps a call to Module::submod_uuids(). Full
     *  documentation can be found there.     *
     *
     *  @return A map from key to UUID for all of the wrapped module's
     *          submodules.
     *
     *  @throw std::runtime_error if the request does not have a module. Strong
     *                            throw guarantee.
     */
    submod_uuid_map submod_uuids() const;

    /** @brief Convenience function for calling uuid on the wrapped module
     *
     *  The ModuleManager assigns UUIDs to modules when they are loaded. This
     *  method is used to retrieve the UUID assigned to the wrapped module. This
     *  method is simply a convenience function for calling Module::uuid() on
     *  the wrapped module.
     *
     *  @return The UUID of the wrapped module.
     *
     *  @throw std::runtime_error if the request does not have a module. Strong
     *                            throw guarantee.
     */
    uuid_type uuid() const;

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

    /** @brief Compares two SubmoduleRequestPIMPL instances for equality
     *
     * Two SubmoduleRequestPIMPL instances are equivalent if they both:
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
    bool operator==(const SubmoduleRequestPIMPL& rhs) const;

    /** @brief Determines if two SubmoduleRequestPIMPL instances are different
     *
     * Two SubmoduleRequestPIMPL instances are equivalent if they both:
     * - request the same property type,
     * - have the same description, and
     * - are not satisfied or are both satisfied with the same module
     *
     * @param[in] rhs The request to compare against
     *
     * @return False if the this instance is the same as @p rhs and true
     *              otherwise.
     *
     * @throw ??? If the comparison between the two modules throws. Same throw
     *        guarantee.
     */
    bool operator!=(const SubmoduleRequestPIMPL& rhs) const;

    bool has_name() const;

    const type::key& get_name() const;

private:
    /// A description of what this submodule will be used for.
    std::optional<type::description> m_desc_;

    /// RTTI of the property type the module must satisfy
    std::optional<std::type_index> m_type_;

    /// Inputs representative of those provided to the required property type
    type::input_map m_inputs_;

    /// The actual submodule
    module_ptr m_module_;
}; // class SubmoduleRequestPIMPL

} // namespace pluginplay::detail_

#include "submodule_request_pimpl.ipp"
