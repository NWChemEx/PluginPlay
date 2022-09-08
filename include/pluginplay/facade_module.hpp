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
#include "pluginplay/module_base.hpp"
#include "pluginplay/types.hpp"
#include <utilities/type_traits/tuple/is_tuple.hpp>

namespace pluginplay {
namespace detail_ {

/** @brief Implements a facade module
 *
 *  A facade module simply returns a value. Notably this means it has no body
 *  and it ignores the inputs (options and submodules) provided to it. This is
 *  useful in at least two scenarios:
 *
 *  - unit testing: we don't need to hook-up a full fledged module, but can
 *    simply make a facade module to mimic the call.
 *  - user input: sometimes the user just wants to specify what a module returns
 *
 *  @tparam PropertyType the property type that the module satisfies.
 */
template<typename PropertyType>
class FacadeModule : public ModuleBase {
private:
    /// The types of the results returned by the property type
    using result_type = decltype(PropertyType::results());
    /// The type of the traits object associated with the results
    using traits_type = typename result_type::traits_type;
    /// A tuple filled with the results
    using result_tuple = typename traits_type::tuple_of_fields;
    /// The number of results
    static constexpr auto n_results = traits_type::nfields;

public:
    /// The type used to return results from the module
    using result_map = type::result_map;

    /// The type used pass inputs to the module
    using input_map = type::input_map;

    /// The type used to pass submodules into the module
    using submodule_map = type::submodule_map;

    /** @brief Creates a FacadeModule by wrapping the provided arguments.
     *
     *  This ctor creates the facade module. If the arguments to the ctor are
     *  not of the right types, or there is too many/few arguments, this class
     *  will fail to compile.
     *
     *  @tparam Args The types of the results that the facade module will
     *               return. The types must be implicitly convertible to the
     *               return types of the property type.
     *  @param[in] args The values that the module will return.
     */
    template<typename... Args>
    explicit FacadeModule(Args&&... args);

    FacadeModule(const FacadeModule&) = delete;
    FacadeModule(FacadeModule&&)      = delete;
    FacadeModule& operator=(const FacadeModule&) = delete;
    FacadeModule& operator=(FacadeModule&&) = delete;

    /** @brief Implements the `run_as` function for the `Module` class.
     *
     *  This function is what is called by the `Module` class's `run_as` and
     *  `run` functions when the Facade module is actually run. This function
     *  simply returns the arguments that the user provied at construction as
     *  results.
     *
     *  @param[in] inputs The type-erased inputs that will be forwarded to the
     *                    user-provided callback. This value will be ignored.
     *  @param[in] submods Facade modules can not have submodules so this is
     *                     expected to be an empty map. This argument will
     *                     ultimately be ignored, but is included to satisfy the
     *                     API. This value will be ignored.
     *
     *  @return A result map which contains the results used to initialize this
     *          .
     */
    result_map run_(input_map, submodule_map) const override;

private:
    /// The inputs to this class, wrapped and ready to be returned.
    result_map m_results_;
}; // class FacadeModule

} // namespace detail_

/** @brief Creates a Facade module by wrapping the provided callback in a
 *         FacadeModule instance.
 *
 *  @tparam PropertyType The property type that this facade satisfies.
 *  @tparam Args         The types of the arguments that the facade module will
 *                       return.
 *
 *  @param[in] args The values that the facade module will return. The order of
 *                  @p args must be the same as the order defined by the
 *                  property type.
 *
 *  @return The Facade module.
 */
template<typename PropertyType, typename... Args>
auto make_facade(Args&&... args);

// ---------------------------Implementations -------------------------------
namespace detail_ {
#define FACADE_MOD_TYPE FacadeModule<PropertyType>

template<typename PropertyType>
template<typename... Args>
FACADE_MOD_TYPE::FacadeModule(Args&&... args) : ModuleBase(this) {
    satisfies_property_type<PropertyType>();
    auto rv    = results();
    m_results_ = PropertyType::wrap_results(rv, std::forward<Args>(args)...);
}

template<typename PropertyType>
typename FACADE_MOD_TYPE::result_map FACADE_MOD_TYPE::run_(
  input_map, submodule_map) const {
    return m_results_;
}

#undef Facade_MOD_TYPE

} // namespace detail_

template<typename PropertyType, typename... Args>
auto make_facade(Args&&... args) {
    using Facade_type = detail_::FacadeModule<PropertyType>;
    auto l  = std::make_unique<Facade_type>(std::forward<Args>(args)...);
    auto lm = Module(std::move(l));
    lm.turn_off_memoization();
    return lm;
}

} // namespace pluginplay
