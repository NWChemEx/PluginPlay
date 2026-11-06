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
#include <cstddef>
#include <optional>
#include <pluginplay/call_graph/wiring_request.hpp>
#include <vector>

namespace pluginplay {

class CallGraphRequest;

/** @brief A chain of branches, the first of which whose condition holds is
 *         applied.
 *
 *  A chain is started by DriverBase::when(input_key, value), which starts its
 *  first branch. Each branch holds wiring requests: fixed values, set with
 *  set_input and set_submod, and inputs and submodules of the driver, which
 *  are forwarded with forward_input and forward_submod. Further branches are
 *  started with otherwise(input_key, value), and a final branch, which applies
 *  if no condition holds, with otherwise().
 *
 *  When the driver is locked, the conditions are tested in the order the
 *  branches were declared, and only the requests of the first branch whose
 *  condition holds are applied. A condition holds if the driver's input
 *  `input_key` equals `value`.
 *
 *  The functions which add requests act on the most recently started branch.
 */
class ConditionalWiring {
public:
    /// Type used for input and submodule keys
    using key_type = WiringRequest::key_type;

    /// Type used for module keys
    using qualified_key = WiringRequest::qualified_key;

    /// Type used for the value a condition tests for
    using value_type = WiringRequest::value_type;

    /// Type of the container holding a branch's requests
    using request_list_type = std::vector<WiringRequest>;

    /// One branch of a chain
    class Branch {
    public:
        /// Does the branch have a condition? False for otherwise()
        bool has_condition() const noexcept { return m_input_key_.has_value(); }

        /// The driver's input the condition tests. Throws if no condition
        const key_type& input_key() const { return m_input_key_.value(); }

        /// The value the input must have. Throws if no condition
        const value_type& value() const;

        /// The requests applied if the branch is taken
        const request_list_type& requests() const noexcept {
            return m_requests_;
        }

        /// Are the requests of the branch required to take effect?
        bool is_required() const noexcept { return m_required_; }

        /// Describes the condition, e.g., `"screen" is true`, for messages
        std::string condition_string() const;

        /// Two branches are equal if their conditions and requests are
        bool operator==(const Branch& rhs) const noexcept;

    private:
        friend class ConditionalWiring;

        /// The driver's input the condition tests (none for otherwise())
        std::optional<key_type> m_input_key_;

        /// The value the input must have
        value_type m_value_;

        /// The requests of the branch
        request_list_type m_requests_;

        /// Are the requests of the branch required?
        bool m_required_ = false;
    };

    /// Type of the container holding the branches
    using branch_list_type = std::vector<Branch>;

    /** @brief Adds a request setting an input to a fixed value.
     *
     *  @tparam T The type of the value.
     *
     *  @param[in] module_key The module whose input is set.
     *  @param[in] input_key The input of @p module_key to set.
     *  @param[in] value The value to set the input to.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::invalid_argument if the target is already set by another
     *                               chain or by DriverInput::set_target.
     *                               Strong throw guarantee.
     */
    template<typename T>
    ConditionalWiring& set_input(qualified_key module_key, key_type input_key,
                                 T&& value) {
        return add_(WiringRequest::make_set_input(
          std::move(module_key), std::move(input_key),
          detail_::make_wiring_value(std::forward<T>(value))));
    }

    /** @brief Adds a request binding a submodule to a module.
     *
     *  @param[in] module_key The module whose submodule is set.
     *  @param[in] callback_key The submodule of @p module_key to set.
     *  @param[in] submod_key The key of the module to bind.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::invalid_argument if the target is already set by another
     *                               chain or by DriverSubmodule::set_target.
     *                               Strong throw guarantee.
     */
    ConditionalWiring& set_submod(qualified_key module_key,
                                  key_type callback_key,
                                  qualified_key submod_key);

    /** @brief Adds a request forwarding one of the driver's inputs.
     *
     *  Works like DriverInput::set_target, but only if the branch is taken.
     *
     *  @param[in] module_key The module whose input is set.
     *  @param[in] target_input_key The input of @p module_key to set.
     *  @param[in] input_key The driver's input whose value is used.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::out_of_range if the driver has no input @p input_key.
     *                           Strong throw guarantee.
     *  @throw std::invalid_argument if the target is already set by another
     *                               chain or by DriverInput::set_target.
     *                               Strong throw guarantee.
     */
    ConditionalWiring& forward_input(qualified_key module_key,
                                     key_type target_input_key,
                                     key_type input_key);

    /** @brief Adds a request forwarding one of the driver's submodules.
     *
     *  Works like DriverSubmodule::set_target, but only if the branch is
     *  taken.
     *
     *  @param[in] module_key The module whose submodule is set.
     *  @param[in] target_callback_key The submodule of @p module_key to set.
     *  @param[in] callback_key The driver's submodule whose module is bound.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::out_of_range if the driver has no submodule
     *                           @p callback_key. Strong throw guarantee.
     *  @throw std::invalid_argument if the target is already set by another
     *                               chain or by DriverSubmodule::set_target.
     *                               Strong throw guarantee.
     */
    ConditionalWiring& forward_submod(qualified_key module_key,
                                      key_type target_callback_key,
                                      key_type callback_key);

    /** @brief Starts a branch which applies if the driver's input
     *         @p input_key equals @p value, and no earlier branch applies.
     *
     *  @tparam T The type of the value.
     *
     *  @param[in] input_key The driver's input to test.
     *  @param[in] value The value the input must have.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::out_of_range if the driver has no input @p input_key.
     *                           Strong throw guarantee.
     *  @throw std::logic_error if otherwise() was already called. Strong
     *                          throw guarantee.
     */
    template<typename T>
    ConditionalWiring& otherwise(key_type input_key, T&& value) {
        return add_branch_(std::move(input_key),
                           detail_::make_wiring_value(std::forward<T>(value)));
    }

    /** @brief Starts the final branch, which applies if no condition holds.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::logic_error if otherwise() was already called. Strong
     *                          throw guarantee.
     */
    ConditionalWiring& otherwise();

    /** @brief Marks the requests of the current branch as required.
     *
     *  Requests added to the branch afterwards are required too. A required
     *  request which would have no effect is an error when the driver is
     *  locked.
     *
     *  @return *this, to support chaining.
     *
     *  @throw None No throw guarantee.
     */
    ConditionalWiring& required() noexcept;

    /// The branches, in the order they were declared. No throw guarantee.
    const branch_list_type& branches() const noexcept { return m_branches_; }

    /// Two chains are equal if their branches are. No throw guarantee.
    bool operator==(const ConditionalWiring& rhs) const noexcept {
        return m_branches_ == rhs.m_branches_;
    }

    /// Negates operator==
    bool operator!=(const ConditionalWiring& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    /// Only CallGraphRequest starts chains
    friend class CallGraphRequest;

    /** @brief Starts a chain with the branch for @p input_key == @p value.
     *
     *  @param[in] owner The request the chain belongs to. Used to check the
     *                   keys of the driver and the targets of the chain.
     *  @param[in] owner_id The ID the chain claims its targets with.
     */
    ConditionalWiring(CallGraphRequest& owner, std::size_t owner_id,
                      key_type input_key, value_type value);

    /// Adds @p request to the current branch
    ConditionalWiring& add_(WiringRequest request);

    /// Starts a branch with a condition
    ConditionalWiring& add_branch_(key_type input_key, value_type value);

    /// The request this chain belongs to
    CallGraphRequest* m_owner_;

    /// The ID this chain claims its targets with
    std::size_t m_owner_id_;

    /// The branches of the chain
    branch_list_type m_branches_;
};

} // namespace pluginplay
