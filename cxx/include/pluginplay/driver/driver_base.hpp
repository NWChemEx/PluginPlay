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
#include <pluginplay/driver/driver_input.hpp>
#include <pluginplay/module/module_base.hpp>
#include <utility>
#include <vector>

namespace pluginplay {

/** @brief The base class of all drivers.
 *
 *  A driver is a module that configures the modules below it. In addition to
 *  what a module's constructor declares, a driver's constructor declares how
 *  the driver's inputs and submodules are passed on to the modules below it
 *  (with set_target on the objects add_input and add_submodule return),
 *  conditional chains (with when), and a memoization policy (with
 *  wire_memoization). These are recorded in the driver's CallGraphRequest,
 *  which the ModuleManager resolves when the driver is locked.
 *
 *  A module is a driver exactly when its implementation derives from
 *  DriverBase. See Module::is_driver.
 */
class DriverBase : public ModuleBase {
public:
    /// Type used for module keys
    using qualified_key = QualifiedKey;

    /// Out of line, so there is a single RTTI for DriverBase
    ~DriverBase() noexcept override;

    /** @brief The developer's request, which the constructor filled.
     *
     *  @return The wiring requests, chains, submodule defaults, and
     *          memoization policy the driver's constructor declared.
     *
     *  @throw None No throw guarantee.
     */
    const CallGraphRequest& call_graph_request() const noexcept override {
        return m_request_;
    }

protected:
    /** @brief The constructor all C++ drivers must call.
     *
     *  @tparam DerivedType The type of the driver deriving from this class.
     *
     *  @param[in] self The `this` pointer of the driver, as for ModuleBase.
     */
    template<typename DerivedType>
    explicit DriverBase(DerivedType* self) noexcept :
      ModuleBase(self), m_request_(*this) {}

    /// The constructor Python drivers must call. See ModuleBase.
    DriverBase(const python::PythonWrapper& self, bool is_python) :
      ModuleBase(self, is_python), m_request_(*this) {}

    /** @brief Adds an input to the driver.
     *
     *  Hides ModuleBase::add_input, so that the returned object can also wire
     *  the new input to the modules below the driver.
     *
     *  @tparam T The type of the input.
     *
     *  @param[in] key The key of the new input.
     *
     *  @return A DriverInput wrapping the new input.
     */
    template<typename T>
    DriverInput add_input(const type::key& key) {
        ModuleBase::add_input<T>(key);
        return driver_input(key);
    }

    /** @brief Adds a submodule to the driver.
     *
     *  Hides ModuleBase::add_submodule, so that the returned object can also
     *  wire the new submodule to the modules below the driver, and name its
     *  default.
     *
     *  @tparam PropertyType The property type of the submodule.
     *
     *  @param[in] key The key of the new submodule.
     *
     *  @return A DriverSubmodule wrapping the new submodule.
     */
    template<typename PropertyType>
    DriverSubmodule add_submodule(const type::key& key) {
        ModuleBase::add_submodule<PropertyType>(key);
        return driver_submodule(key);
    }

    /** @brief Wraps an input which has already been added.
     *
     *  @param[in] key The key of the input.
     *
     *  @return A DriverInput wrapping the input.
     *
     *  @throw std::out_of_range if the driver has no input @p key. Strong
     *                           throw guarantee.
     */
    DriverInput driver_input(const type::key& key) {
        return DriverInput(key, inputs().at(key), m_request_);
    }

    /** @brief Wraps a submodule which has already been added.
     *
     *  @param[in] key The key of the submodule.
     *
     *  @return A DriverSubmodule wrapping the submodule.
     *
     *  @throw std::out_of_range if the driver has no submodule @p key. Strong
     *                           throw guarantee.
     */
    DriverSubmodule driver_submodule(const type::key& key) {
        return DriverSubmodule(key, submods().at(key), m_request_);
    }

    /** @brief Starts a chain whose first branch applies if the driver's input
     *         @p input_key equals @p value.
     *
     *  @tparam T The type of the value.
     *
     *  @param[in] input_key The driver's input to test.
     *  @param[in] value The value the input must have.
     *
     *  @return The new chain, to which requests and branches are added.
     *
     *  @throw std::out_of_range if the driver has no input @p input_key.
     *                           Strong throw guarantee.
     */
    template<typename T>
    ConditionalWiring& when(type::key input_key, T&& value) {
        return m_request_.when(std::move(input_key), detail_::make_wiring_value(
                                                       std::forward<T>(value)));
    }

    /** @brief Requests whether the modules below the driver memoize.
     *
     *  @param[in] on Whether the modules below the driver should memoize.
     *  @param[in] except The modules which get the opposite setting.
     */
    void wire_memoization(bool on, std::vector<qualified_key> except = {}) {
        m_request_.wire_memoization(on, std::move(except));
    }

private:
    /// The developer's request
    CallGraphRequest m_request_;
};

} // namespace pluginplay
