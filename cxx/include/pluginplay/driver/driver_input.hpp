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
#include <pluginplay/call_graph/call_graph_request.hpp>
#include <pluginplay/fields/module_input.hpp>
#include <pluginplay/submodule_request.hpp>
#include <utility>

namespace pluginplay {

/** @brief An input of a driver, as it is declared.
 *
 *  DriverBase::add_input returns a DriverInput, rather than the ModuleInput
 *  ModuleBase::add_input returns. It has the chainable setters of
 *  ModuleInput, which forward to the input, and adds set_target and
 *  set_required_target, which request that the value of the input is also
 *  set on a module below the driver.
 *
 *  A DriverInput holds references to the input and to the driver's request.
 *  It holds no state of its own and can be discarded once the chain ends.
 */
class DriverInput {
public:
    /// Type used for module keys
    using qualified_key = QualifiedKey;

    /// Sets the default value of the input. See ModuleInput::set_default
    template<typename T>
    DriverInput& set_default(T&& value) {
        m_input_->set_default(std::forward<T>(value));
        return *this;
    }

    /// Sets the description of the input. See ModuleInput::set_description
    DriverInput& set_description(type::description desc) noexcept {
        m_input_->set_description(std::move(desc));
        return *this;
    }

    /// Adds a bounds check to the input. See ModuleInput::add_check
    template<typename... Args>
    DriverInput& add_check(Args&&... args) {
        m_input_->add_check(std::forward<Args>(args)...);
        return *this;
    }

    /// Makes the input optional. See ModuleInput::make_optional
    DriverInput& make_optional() noexcept {
        m_input_->make_optional();
        return *this;
    }

    /// Makes the input required. See ModuleInput::make_required
    DriverInput& make_required() noexcept {
        m_input_->make_required();
        return *this;
    }

    /// Makes the input opaque. See ModuleInput::make_opaque
    DriverInput& make_opaque() noexcept {
        m_input_->make_opaque();
        return *this;
    }

    /// Makes the input transparent. See ModuleInput::make_transparent
    DriverInput& make_transparent() noexcept {
        m_input_->make_transparent();
        return *this;
    }

    /** @brief Requests that the value of this input is also set as an input
     *         of a module below the driver.
     *
     *  May be called several times, once per target.
     *
     *  @param[in] module_key The module below the driver whose input is set.
     *  @param[in] target_input_key The input of @p module_key to set.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::invalid_argument if the target is already set by another
     *                               set_target or chain of the driver, or if
     *                               the input is an input of a property type
     *                               the driver satisfies. Strong throw
     *                               guarantee.
     */
    DriverInput& set_target(qualified_key module_key,
                            type::key target_input_key) {
        m_request_->forward_input(std::move(module_key),
                                  std::move(target_input_key), m_key_);
        return *this;
    }

    /** @brief Same as set_target, but the request must take effect.
     *
     *  If the target is not in the call graph when the driver is locked,
     *  locking throws instead of reporting the setting as inactive.
     *
     *  @param[in] module_key The module below the driver whose input is set.
     *  @param[in] target_input_key The input of @p module_key to set.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::invalid_argument if the target is already set by another
     *                               set_target or chain of the driver, or if
     *                               the input is an input of a property type
     *                               the driver satisfies. Strong throw
     *                               guarantee.
     */
    DriverInput& set_required_target(qualified_key module_key,
                                     type::key target_input_key) {
        m_request_
          ->forward_input(std::move(module_key), std::move(target_input_key),
                          m_key_)
          .required();
        return *this;
    }

    /// The input being declared. No throw guarantee.
    ModuleInput& input() noexcept { return *m_input_; }

private:
    /// Only DriverBase makes these
    friend class DriverBase;

    /// Wraps the driver's input @p key, which is @p input
    DriverInput(type::key key, ModuleInput& input,
                CallGraphRequest& request) noexcept :
      m_key_(std::move(key)), m_input_(&input), m_request_(&request) {}

    /// The key of the input
    type::key m_key_;

    /// The input
    ModuleInput* m_input_;

    /// The developer's request of the driver
    CallGraphRequest* m_request_;
};

/** @brief A submodule of a driver, as it is declared.
 *
 *  DriverBase::add_submodule returns a DriverSubmodule, rather than the
 *  SubmoduleRequest ModuleBase::add_submodule returns. It adds set_default,
 *  which names the module bound to the submodule if the user binds none, and
 *  set_target and set_required_target, which request that the module bound to
 *  the submodule is also bound to a submodule of a module below the driver.
 */
class DriverSubmodule {
public:
    /// Type used for module keys
    using qualified_key = QualifiedKey;

    /// Sets the description. See SubmoduleRequest::set_description
    DriverSubmodule& set_description(type::description desc) noexcept {
        m_submod_->set_description(std::move(desc));
        return *this;
    }

    /** @brief Names the module bound to the submodule if the user binds none.
     *
     *  Modules are found by key, so the submodule can not be bound in the
     *  driver's constructor. The default is recorded in the driver's request
     *  and bound by the ModuleManager when it resolves the driver's requests,
     *  if the submodule is still unbound. It should be a module of the
     *  driver's own plugin.
     *
     *  @param[in] submod_key The key of the default module.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::bad_alloc if there is a problem recording the default.
     *                        Strong throw guarantee.
     */
    DriverSubmodule& set_default(qualified_key submod_key) {
        m_request_->set_submod_default(m_key_, std::move(submod_key));
        return *this;
    }

    /** @brief Requests that the module bound to this submodule is also bound
     *         to a submodule of a module below the driver.
     *
     *  May be called several times, once per target.
     *
     *  @param[in] module_key The module below the driver whose submodule is
     *                        set.
     *  @param[in] target_callback_key The submodule of @p module_key to set.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::invalid_argument if the target is already set by another
     *                               set_target or chain of the driver. Strong
     *                               throw guarantee.
     */
    DriverSubmodule& set_target(qualified_key module_key,
                                type::key target_callback_key) {
        m_request_->forward_submod(std::move(module_key),
                                   std::move(target_callback_key), m_key_);
        return *this;
    }

    /** @brief Same as set_target, but the request must take effect.
     *
     *  @param[in] module_key The module below the driver whose submodule is
     *                        set.
     *  @param[in] target_callback_key The submodule of @p module_key to set.
     *
     *  @return *this, to support chaining.
     *
     *  @throw std::invalid_argument if the target is already set by another
     *                               set_target or chain of the driver. Strong
     *                               throw guarantee.
     */
    DriverSubmodule& set_required_target(qualified_key module_key,
                                         type::key target_callback_key) {
        m_request_
          ->forward_submod(std::move(module_key),
                           std::move(target_callback_key), m_key_)
          .required();
        return *this;
    }

    /// The submodule being declared. No throw guarantee.
    SubmoduleRequest& submodule() noexcept { return *m_submod_; }

private:
    /// Only DriverBase makes these
    friend class DriverBase;

    /// Wraps the driver's submodule @p key, which is @p submod
    DriverSubmodule(type::key key, SubmoduleRequest& submod,
                    CallGraphRequest& request) noexcept :
      m_key_(std::move(key)), m_submod_(&submod), m_request_(&request) {}

    /// The key of the submodule
    type::key m_key_;

    /// The submodule
    SubmoduleRequest* m_submod_;

    /// The developer's request of the driver
    CallGraphRequest* m_request_;
};

} // namespace pluginplay
