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
#include <deque>
#include <optional>
#include <pluginplay/call_graph/conditional_wiring.hpp>
#include <pluginplay/call_graph/wiring_request.hpp>
#include <utilities/containers/case_insensitive_map.hpp>
#include <vector>

namespace pluginplay {

class ModuleBase;
class DriverBase;
class DriverInput;
class DriverSubmodule;

namespace detail_ {
class ModulePIMPL;
} // namespace detail_

/** @brief What a driver sets on the modules below it.
 *
 *  A CallGraphRequest is an ordered list of wiring requests (see
 *  WiringRequest), plus the conditional chains (see ConditionalWiring), the
 *  default modules of the driver's submodules, and the memoization policy of
 *  a driver. It is data: it names its targets by key and resolves nothing
 *  itself. The ModuleManager resolves it when the driver is locked.
 *
 *  A driver has two requests:
 *
 *  - the developer's, filled in the driver's constructor through DriverBase,
 *    DriverInput, and DriverSubmodule; and
 *  - the user's, returned by Module::call_graph_request(), to which the user
 *    may only add fixed values, with set_input and set_submod.
 *
 *  The user's requests are applied after the developer's, so they win.
 *
 *  A request is locked while the driver holding it is, i.e., once the driver
 *  has run. Changing a locked request throws, since the driver's requests are
 *  only resolved before it locks.
 */
class CallGraphRequest {
public:
    /// Type used for input and submodule keys
    using key_type = WiringRequest::key_type;

    /// Type used for module keys
    using qualified_key = WiringRequest::qualified_key;

    /// Type of the container holding the requests
    using request_list_type = std::deque<WiringRequest>;

    /// Type of the container holding the conditional chains
    using chain_list_type = std::deque<ConditionalWiring>;

    /// Type of the map from the driver's submodules to their default modules
    using submod_default_map = utilities::CaseInsensitiveMap<qualified_key>;

    /// Whether the modules below a driver memoize
    struct MemoizationPolicy {
        /// Should the modules memoize?
        bool on = true;

        /// The modules which get the opposite setting
        std::vector<qualified_key> except;

        /// Two policies are equal if their members are
        bool operator==(const MemoizationPolicy& rhs) const noexcept {
            return on == rhs.on && except == rhs.except;
        }
    };

    /// Type of the (optional) memoization policy
    using memoization_policy_type = std::optional<MemoizationPolicy>;

    /// Makes an empty request, e.g., the user's request of a driver
    CallGraphRequest() = default;

    /// Deep copies @p rhs
    CallGraphRequest(const CallGraphRequest& rhs);

    /// Takes the state of @p rhs
    CallGraphRequest(CallGraphRequest&& rhs) noexcept;

    /// Replaces the state of *this with a deep copy of @p rhs
    CallGraphRequest& operator=(const CallGraphRequest& rhs);

    /// Replaces the state of *this with the state of @p rhs
    CallGraphRequest& operator=(CallGraphRequest&& rhs) noexcept;

    /// Default no-throw dtor
    ~CallGraphRequest() noexcept = default;

    /** @brief Requests that an input of a module below the driver is set to
     *         @p value.
     *
     *  @tparam T The type of the value.
     *
     *  @param[in] module_key The module whose input is set.
     *  @param[in] input_key The input of @p module_key to set.
     *  @param[in] value The value to set the input to.
     *
     *  @return The new request, so that it can be marked as required.
     *
     *  @throw std::runtime_error if *this is locked. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the request.
     *                        Strong throw guarantee.
     */
    template<typename T>
    WiringRequest& set_input(qualified_key module_key, key_type input_key,
                             T&& value) {
        assert_unlocked_();
        return add_(WiringRequest::make_set_input(
          std::move(module_key), std::move(input_key),
          detail_::make_wiring_value(std::forward<T>(value))));
    }

    /** @brief Requests that a submodule of a module below the driver is bound
     *         to the module with the key @p submod_key.
     *
     *  @param[in] module_key The module whose submodule is set.
     *  @param[in] callback_key The submodule of @p module_key to set.
     *  @param[in] submod_key The key of the module to bind.
     *
     *  @return The new request, so that it can be marked as required.
     *
     *  @throw std::runtime_error if *this is locked. Strong throw guarantee.
     *  @throw std::bad_alloc if there is a problem allocating the request.
     *                        Strong throw guarantee.
     */
    WiringRequest& set_submod(qualified_key module_key, key_type callback_key,
                              qualified_key submod_key);

    /// Is *this locked, i.e., can it no longer be changed? No throw guarantee.
    bool is_locked() const noexcept { return m_locked_; }

    /// The requests outside of chains, in the order they were made
    const request_list_type& requests() const noexcept { return m_requests_; }

    /// The conditional chains, in the order they were started
    const chain_list_type& conditional_wiring() const noexcept {
        return m_chains_;
    }

    /// The default modules of the driver's submodules
    const submod_default_map& submod_defaults() const noexcept {
        return m_submod_defaults_;
    }

    /// The memoization policy, if one was requested
    const memoization_policy_type& memoization_policy() const noexcept {
        return m_memoization_;
    }

    /// True if nothing has been requested and false otherwise
    bool empty() const noexcept {
        return m_requests_.empty() && m_chains_.empty() &&
               m_submod_defaults_.empty() && !m_memoization_.has_value();
    }

    /** @brief Determines if *this and @p rhs request the same things.
     *
     *  @param[in] rhs The request to compare to.
     *
     *  @return True if *this and @p rhs hold the same requests, chains,
     *          defaults, and memoization policy, and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const CallGraphRequest& rhs) const noexcept;

    /// Negates operator==
    bool operator!=(const CallGraphRequest& rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    // Only drivers, through the classes below, can forward their inputs and
    // submodules, and start chains
    friend class DriverBase;
    friend class DriverInput;
    friend class DriverSubmodule;
    friend class ConditionalWiring;

    // Requests check whether they can be changed with assert_unlocked_
    friend class WiringRequest;

    // The module holding *this locks and unlocks it
    friend class detail_::ModulePIMPL;

    /// Makes the developer's request of @p driver, whose keys are checked
    explicit CallGraphRequest(const ModuleBase& driver) noexcept :
      m_driver_(&driver) {}

    /// Forwards the driver's input @p input_key to a module below the driver
    WiringRequest& forward_input(qualified_key module_key,
                                 key_type target_input_key, key_type input_key);

    /// Forwards the driver's submodule @p callback_key to a module below it
    WiringRequest& forward_submod(qualified_key module_key,
                                  key_type target_callback_key,
                                  key_type callback_key);

    /// Starts a chain whose first branch applies if @p input_key == @p value
    ConditionalWiring& when(key_type input_key, type::any value);

    /// Requests whether the modules below the driver memoize
    void wire_memoization(bool on, std::vector<qualified_key> except = {});

    /// Sets the module bound to the driver's submodule if the user binds none
    void set_submod_default(key_type callback_key, qualified_key submod_key);

    /// Adds a request outside of any chain
    WiringRequest& add_(WiringRequest request);

    /// Keeps *this from being changed, e.g., because its driver is locked
    void lock() noexcept { m_locked_ = true; }

    /// Allows *this to be changed again
    void unlock() noexcept { m_locked_ = false; }

    /// Throws if *this is locked
    void assert_unlocked_() const;

    /// Throws if the driver has no input @p key
    void assert_driver_input_(const key_type& key) const;

    /// Throws if the driver has no submodule @p key
    void assert_driver_submod_(const key_type& key) const;

    /// Records that @p owner sets the target of @p request, or throws
    void claim_(const WiringRequest& request, std::size_t owner);

    /// Points the requests and chains at *this, after a copy or move
    void adopt_children_() noexcept;

    /// One target claimed by a set_target or a chain
    struct Claim {
        qualified_key module_key;
        bool is_input;
        key_type target_key;
        std::size_t owner;
    };

    /// The driver whose keys are checked, if any
    const ModuleBase* m_driver_ = nullptr;

    /// The requests outside of chains
    request_list_type m_requests_;

    /// The conditional chains
    chain_list_type m_chains_;

    /// The default modules of the driver's submodules
    submod_default_map m_submod_defaults_;

    /// The memoization policy
    memoization_policy_type m_memoization_;

    /// The targets set by the developer, with what set them
    std::vector<Claim> m_claims_;

    /// The ID the next set_target or chain claims its targets with
    std::size_t m_next_owner_ = 0;

    /// Can *this no longer be changed?
    bool m_locked_ = false;
};

} // namespace pluginplay
