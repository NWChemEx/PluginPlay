/*
 * Copyright 2026 NWChemEx-Project
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
#include <pluginplay/any/any.hpp>
#include <pluginplay/module_manager/qualified_key.hpp>
#include <pluginplay/types.hpp>
#include <string>
#include <type_traits>
#include <utility>

namespace pluginplay {
class CallGraphRequest;

namespace detail_ {

/** @brief Wraps a value a driver sets in a type-erased AnyField.
 *
 *  By convention, PluginPlay stores strings as std::string, so C-strings are
 *  converted. Values which are already type-erased are returned as is.
 *
 *  @tparam T The type of the value.
 *
 *  @param[in] value The value to wrap.
 *
 *  @return @p value, wrapped in an AnyField.
 */
template<typename T>
type::any make_wiring_value(T&& value) {
    using clean_type = std::decay_t<T>;
    if constexpr(std::is_same_v<clean_type, type::any>) {
        return std::forward<T>(value);
    } else if constexpr(std::is_same_v<clean_type, const char*> ||
                        std::is_same_v<clean_type, char*>) {
        return any::make_any_field<std::string>(std::string(value));
    } else {
        return any::make_any_field<clean_type>(std::forward<T>(value));
    }
}

} // namespace detail_

/** @brief One input or submodule a driver sets on a module below it.
 *
 *  A wiring request names a *target*, i.e., a module key and either an input or
 *  submodule of that module, and what the target should be set to. There are
 *  four kinds of requests:
 *
 *  - set_input sets the target input to a fixed value,
 *  - set_submod binds the target submodule to the module with a given key,
 *  - forward_input sets the target input to the value of one of the
 *    driver's inputs, and
 *  - forward_submod binds the target submodule to the module bound to one of
 *    the driver's submodules.
 *
 *  Wiring requests are data. They name modules by key, and are resolved by
 *  the ModuleManager when the driver is locked. Requests are made through
 *  CallGraphRequest, ConditionalWiring, DriverInput, and DriverSubmodule.
 */
class WiringRequest {
public:
    /// The kinds of wiring requests
    enum class kind_type {
        set_input,
        set_submod,
        forward_input,
        forward_submod
    };

    /// Type used for input and submodule keys
    using key_type = type::key;

    /// Type used for module keys
    using qualified_key = QualifiedKey;

    /// Type used to hold the value of a set_input request
    using value_type = type::any;

    /** @brief Makes a request setting an input to a fixed value.
     *
     *  @param[in] module_key The module whose input is set.
     *  @param[in] input_key The input of @p module_key to set.
     *  @param[in] value The value to set the input to.
     *
     *  @return The new request.
     *
     *  @throw std::bad_alloc if there is a problem allocating the request.
     *                        Strong throw guarantee.
     */
    static WiringRequest make_set_input(qualified_key module_key,
                                        key_type input_key, value_type value);

    /** @brief Makes a request binding a submodule to a module.
     *
     *  @param[in] module_key The module whose submodule is set.
     *  @param[in] callback_key The submodule of @p module_key to set.
     *  @param[in] submod_key The key of the module to bind.
     *
     *  @return The new request.
     *
     *  @throw std::bad_alloc if there is a problem allocating the request.
     *                        Strong throw guarantee.
     */
    static WiringRequest make_set_submod(qualified_key module_key,
                                         key_type callback_key,
                                         qualified_key submod_key);

    /** @brief Makes a request forwarding one of the driver's inputs.
     *
     *  @param[in] module_key The module whose input is set.
     *  @param[in] target_input_key The input of @p module_key to set.
     *  @param[in] input_key The driver's input whose value is used.
     *
     *  @return The new request.
     *
     *  @throw std::bad_alloc if there is a problem allocating the request.
     *                        Strong throw guarantee.
     */
    static WiringRequest make_forward_input(qualified_key module_key,
                                            key_type target_input_key,
                                            key_type input_key);

    /** @brief Makes a request forwarding one of the driver's submodules.
     *
     *  @param[in] module_key The module whose submodule is set.
     *  @param[in] target_callback_key The submodule of @p module_key to set.
     *  @param[in] callback_key The driver's submodule whose module is bound.
     *
     *  @return The new request.
     *
     *  @throw std::bad_alloc if there is a problem allocating the request.
     *                        Strong throw guarantee.
     */
    static WiringRequest make_forward_submod(qualified_key module_key,
                                             key_type target_callback_key,
                                             key_type callback_key);

    /** @brief Marks *this as a request which must take effect.
     *
     *  A request which would have no effect, e.g., because its target is not
     *  in the call graph, is normally reported as inactive. If the request is
     *  required, locking the driver throws instead.
     *
     *  @param[in] value Whether the request is required. Defaults to true.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::runtime_error if *this belongs to a CallGraphRequest which
     *                            is locked. Strong throw guarantee.
     */
    WiringRequest& required(bool value = true);

    /// Must *this take effect? No throw guarantee.
    bool is_required() const noexcept { return m_required_; }

    /// What kind of request is *this? No throw guarantee.
    kind_type kind() const noexcept { return m_kind_; }

    /// True if *this sets an input, false if it sets a submodule
    bool sets_input() const noexcept {
        return m_kind_ == kind_type::set_input ||
               m_kind_ == kind_type::forward_input;
    }

    /// True if *this forwards an input or submodule of the driver
    bool is_forward() const noexcept {
        return m_kind_ == kind_type::forward_input ||
               m_kind_ == kind_type::forward_submod;
    }

    /// The module whose input or submodule *this sets. No throw guarantee.
    const qualified_key& module_key() const noexcept { return m_module_key_; }

    /// The input or submodule of module_key() *this sets. No throw guarantee.
    const key_type& target_key() const noexcept { return m_target_key_; }

    /** @brief The value a set_input request sets.
     *
     *  @return The value the target input is set to.
     *
     *  @throw std::runtime_error if *this is not a set_input request. Strong
     *                            throw guarantee.
     */
    const value_type& value() const;

    /** @brief The key of the module a set_submod request binds.
     *
     *  @return The key of the module bound to the target submodule.
     *
     *  @throw std::runtime_error if *this is not a set_submod request. Strong
     *                            throw guarantee.
     */
    const qualified_key& submod_key() const;

    /** @brief The driver's input or submodule a forward request uses.
     *
     *  @return The key of the driver's input or submodule.
     *
     *  @throw std::runtime_error if *this is not a forward request. Strong
     *                            throw guarantee.
     */
    const key_type& source_key() const;

    /** @brief Describes *this for messages.
     *
     *  @return A human-readable description of *this, e.g.,
     *          `setting "Module A"'s input "Tolerance" to 1e-08`.
     *
     *  @throw std::bad_alloc if there is a problem allocating the result.
     *                        Strong throw guarantee.
     */
    std::string to_string() const;

    /** @brief Determines if *this and @p rhs are the same request.
     *
     *  Two requests are the same if they are of the same kind, have the same
     *  target, set it to the same value, and are both required or both not.
     *
     *  @param[in] rhs The request to compare to.
     *
     *  @return True if *this and @p rhs are the same and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const WiringRequest& rhs) const noexcept;

    /// Negates operator==
    bool operator!=(const WiringRequest& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    // Points the requests it holds at itself
    friend class CallGraphRequest;

    /** @brief A pointer to the CallGraphRequest holding a request.
     *
     *  A copy of a request is not held by the CallGraphRequest of the
     *  original, so copying (or moving) an OwnerPointer gives a null pointer,
     *  and assigning one leaves it as is. The CallGraphRequest sets it.
     */
    struct OwnerPointer {
        OwnerPointer() noexcept = default;
        OwnerPointer(const OwnerPointer&) noexcept {}
        OwnerPointer& operator=(const OwnerPointer&) noexcept { return *this; }

        /// The CallGraphRequest, or nullptr if no CallGraphRequest holds it
        const CallGraphRequest* value = nullptr;
    };

    /// Used by the make_* functions
    WiringRequest(kind_type kind, qualified_key module_key,
                  key_type target_key) :
      m_kind_(kind),
      m_module_key_(std::move(module_key)),
      m_target_key_(std::move(target_key)) {}

    /// Code factorization for checking the kind of *this
    void assert_kind_(bool is_kind, const char* what) const;

    /// What kind of request this is
    kind_type m_kind_;

    /// The module whose input or submodule is set
    qualified_key m_module_key_;

    /// The input or submodule which is set
    key_type m_target_key_;

    /// The value of a set_input request
    value_type m_value_;

    /// The module a set_submod request binds
    qualified_key m_submod_key_{key_type{}};

    /// The driver's input or submodule a forward request uses
    key_type m_source_key_;

    /// Must the request take effect?
    bool m_required_ = false;

    /// The CallGraphRequest which holds *this, if any
    OwnerPointer m_owner_;
};

} // namespace pluginplay
