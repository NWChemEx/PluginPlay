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
#include <pluginplay/module.hpp>
#include <pluginplay/module_base.hpp>
#include <pluginplay/types.hpp>
#include <pluginplay/utility/uuid.hpp>
#include <utilities/type_traits/tuple/is_tuple.hpp>

namespace pluginplay {
namespace detail_ {

/** @brief Implements a lambda module
 *
 *  Lambda modules generalize the idea of a C++/Python lambda to modules. In
 *  other words, a lambda module is a module that can be made on the spot, at
 *  runtime, using runtime state. Lambda modules are useful in a variety of
 *  circumstances including:
 *
 *  - user input
 *    - user's write a function which returns the input to another module
 *  - rapid prototyping
 *    - developers can implement a module without specifying all of the meta-
 *      data
 *  - unit-testing
 *    - developers can decouple unit testing of a module from the submodules it
 *      calls by writing a lambda module that returns a hard-coded value.
 *  - module implementations
 *    - many algorithms can be written as slightly modified versions of other
 *      algorithms.
 *    - lambda modules provide a straightforward way to adjust an already
 *      existing algorithm's submodules
 *
 *  Lambda modules are envisioned as relatively simple modules that are not
 *  meant to be distributed as modules on their own. In particular this means
 *  lambda modules do **not**:
 *
 *  - have metadata associated with them,
 *  - take additional arguments beyond those defined by the property type they
 *    satisfy,
 *  - have submodules,
 *  - return additional values beyond those defined by the property type,
 *  - have caches associated with them, or
 *  - have their results memoized.
 *
 *  The LambdaModule class implements the ModuleBase API by wrapping a
 *  user-provided callback (lambda function, funtor, or plain-old function) in a
 *  C++ lambda with the generic `run` API. This is far easier than trying to
 *  capture the user-provided function directly as it allows us to avoid
 *  nuances in the user's function's API which may rely on things such as
 *  implicit conversions to satisfy `PropertyType`'s API.
 *
 *  @note Copy/move construction/assignment is delted by ModuleBase and
 *        keeping them disabled for LambdaModule also helps prevent unintended
 *        usage.
 *
 *  @tparam PropertyType The type of the property type that the lambda module
 *                       satisfies. Expected to be a class that derives from
 *                       the `PropertyType` class.
 */
template<typename PropertyType>
class LambdaModule : public ModuleBase {
public:
    /// The type used to return results from the module
    using result_map = type::result_map;

    /// The type used pass inputs to the module
    using input_map = type::input_map;

    /// The type used to pass submodules into the module
    using submodule_map = type::submodule_map;

    /** @brief Creates a LambdaModule by wrapping the provided callback.
     *
     *  @tparam FxnType The type of the user-provided callback. Assumed to be a
     *                  callable type with an API that is compatible with the
     *                  API defined by @p PropertyType.
     *
     *  @param[in] fxn The user-provided callback which will be used to
     *                 implement this module.
     */
    template<typename FxnType>
    explicit LambdaModule(FxnType&& fxn);

    LambdaModule(const LambdaModule&) = delete;
    LambdaModule(LambdaModule&&)      = delete;
    LambdaModule& operator=(const LambdaModule&) = delete;
    LambdaModule& operator=(LambdaModule&&) = delete;

    /** @brief Implements the `run_as` function for the `Module` class.
     *
     *  This function is what is called by the `Module` class's `run_as` and
     *  `run` functions when the lambda module is actually run. This function
     *  simply unwraps the inputs, forwards them to the callback, and then wraps
     *  and returns the results.
     *
     *  @param[in] inputs The type-erased inputs that will be forwarded to the
     *                    user-provided callback.
     *  @param[in] submods Lambda modules can not have submodules so this is
     *                     expected to be an empty map. This argument will
     *                     ultimately be ignored, but is included to satisfy the
     *                     API.
     *
     *  @return A result map which contains the results returned from the user-
     *          provided callback in a type-erased manner.
     *
     *  @throw ??? If the user-provided callback throws. Same guarantee.
     */
    result_map run_(input_map inputs, submodule_map submods) const override;

private:
    /** @brief Unpacks the tuple of returns into PropertyType::wrap_results
     *
     *  This function is basically `std::apply`, but we can't use `std::apply`
     *  here because `std::apply(PropertyType::wrap_results, rv)` can't
     *  implicitly determine the template type parameters for
     *  `PropertyType::wrap_results`. We could use TMP to deduce them, but that
     *  ends up being more work than just writing our own `apply` for this case.
     *
     *  @tparam T The type of the tuple we are unpacking
     *  @tparam Is The integers 0 to (std::tuple_size_v<T> - 1)
     *
     *  @param[in] rv The tuple to unpack into `PropertyType::wrap_results`.
     *
     *  @return The `result_map` instance resulting from wrapping @p rv.
     */
    template<typename T, std::size_t... Is>
    auto wrap_results_(T&& rv, std::index_sequence<Is...>);

    /// The user-provided callback wrapped for use with the `run_` member
    std::function<result_map(input_map, const submodule_map&)> m_fxn_;
}; // class LambdaModule

} // namespace detail_

/** @brief Creates a lambda module by wrapping the provided callback in a
 *         LambdaModule instance.
 *
 *  @relates detail_::LambdaModule
 *
 *  This is the public API for creating a lambda module, it requires you to
 *  specify the property type that the module satisfies and provide a callback
 *  to implement the lambda module. Typical usage is something like:
 *
 *  ```
 *  Module my_lambda = make_lambda<PropertyType>([](){ ...});
 *  ```
 *
 *  where `PropertyType` would be replaced with the class type defining the API
 *  of the property your module computes and the C++ lambda is the callback the
 *  resulting lambda module will wrap.
 *
 *  @tparam PropertyType The class defining the API of the property type the
 *                       lambda module implements. This template type parameter
 *                       must be explicitly specified by the user and can not be
 *                       inferred.
 *  @tparam FxnType The type of the callback, which will be wrapped inside the
 *                  resulting lambda module. This template type parameter will
 *                  be inferred and need not be specified.
 *
 *  @param[in] fxn The callback which will be used to implement the resulting
 *                 lambda module.
 *
 *  @return The lambda module.
 */
template<typename PropertyType, typename FxnType>
auto make_lambda(FxnType&& fxn);

// ---------------------------Implementations -------------------------------
namespace detail_ {
#define LAMBDA_MOD_TYPE LambdaModule<PropertyType>

template<typename PropertyType>
template<typename FxnType>
LAMBDA_MOD_TYPE::LambdaModule(FxnType&& fxn) :
  ModuleBase(this),
  m_fxn_([&, da_fxn = std::forward<FxnType>(fxn)](
           input_map inputs, const submodule_map&) -> result_map {
      auto unwrapped_inputs = PropertyType::unwrap_inputs(std::move(inputs));

      using result_type        = decltype(PropertyType::results());
      using traits_type        = typename result_type::traits_type;
      using result_tuple       = typename traits_type::tuple_of_fields;
      constexpr auto n_results = traits_type::nfields;
      result_tuple rv;

      if constexpr(n_results > 0) {
          rv = std::move(std::apply(da_fxn, std::move(unwrapped_inputs)));
      } else {
          std::apply(da_fxn, std::move(unwrapped_inputs));
      }

      auto is = std::make_index_sequence<n_results>();

      return wrap_results_(std::move(rv), is);
  } // end lambda function
  ) {
    satisfies_property_type<PropertyType>();
}

template<typename PropertyType>
typename LAMBDA_MOD_TYPE::result_map LAMBDA_MOD_TYPE::run_(
  input_map inputs, submodule_map submods) const {
    return m_fxn_(std::move(inputs), std::move(submods));
}

template<typename PropertyType>
template<typename T, std::size_t... Is>
auto LAMBDA_MOD_TYPE::wrap_results_(T&& rvs, std::index_sequence<Is...>) {
    result_map rv_map = results();
    return PropertyType::wrap_results(std::move(rv_map), std::get<Is>(rvs)...);
}
#undef LAMBDA_MOD_TYPE

} // namespace detail_

template<typename PropertyType, typename FxnType>
auto make_lambda(FxnType&& fxn) {
    using lambda_type = detail_::LambdaModule<PropertyType>;
    auto l = std::make_unique<lambda_type>(std::forward<FxnType>(fxn));
    l->set_uuid(utility::generate_uuid());
    auto lm = Module(std::move(l));
    return lm;
}

} // namespace pluginplay
