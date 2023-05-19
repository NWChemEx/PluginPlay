/*
 * Copyright 2023 NWChemEx-Project
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
#include <pluginplay/python/python_wrapper.hpp>
#include <pluginplay/types.hpp>
#include <type_traits>

namespace pluginplay::python {
namespace detail_ {

/** @brief Implements py_wrap_results
 *
 *  @note Calling this function without pybind11 support enabled will result in
 *        an exception.
 *
 *  @tparam PT The type of the property type we are implementing.
 *  @tparam ArgsType A type representing Python's "args" concept. It is expected
 *                   to be pybind11::args, but this function is templated on the
 *                   type to avoid introducing pybind11.
 *
 *  @tparam I A template parameter pack containing the indices of @p args we
 *            want to unpack.
 *
 *  @param[in] results The ResultMap instance we are populating.
 *  @param[in] args The results we are adding to @p results.
 *  @param[in] <anonymous> An object used purely for it's template non-type
 *             parameters.
 *
 *  @return @p results after populating it from @p args.
 */
template<typename PT, typename ArgsType, std::size_t... I>
auto py_wrap_results_(type::result_map& results, ArgsType&& args,
                      std::index_sequence<I...>) {
    using pluginplay::python::PythonWrapper;
    return PT::wrap_results(results,
                            any::make_any_field<PythonWrapper>(args[I])...);
}

} // namespace detail_

/** @brief Used in the Python bindings of property types.
 *
 *  This method forms the guts of the Python bindings for the `wrap_results`
 *  method. It will unpack the variadic @p args parameter so that the i-th
 *  argument in @p args is used as the i-th result.
 *
 *  @note `py_wrap_results` is templated on @p ArgsType so that it will compile
 *        even if pybind11 support is not enabled. If pybind11 support is not
 *        enabled, then attempting to call this function will result in a
 *        runtime error.
 *
 *  @tparam PT The type of the property type we are wrapping results for.
 *  @tparam ArgsType The type of the object representing Python's "args"
 *          concept. @p ArgsType is assumed to be pybind11::args.
 *
 *  @param[in] results The `type::result_map` object we are filling in.
 *  @param[in] args The results passed in to the property type from Python.
 *
 *  @return @p results after filling it in based on @p args.
 */
template<typename PT, typename ArgsType>
auto py_wrap_results(type::result_map& results, ArgsType&& args) {
    using result_tuple_type = decltype(PT::results());
    constexpr auto n        = result_tuple_type::size();
    constexpr auto indices  = std::make_index_sequence<n>();
    return detail_::py_wrap_results_<PT>(results, std::forward<ArgsType>(args),
                                         indices);
}

} // namespace pluginplay::python
