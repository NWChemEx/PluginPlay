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

/** @brief Implements py_wrap_inputs
 *
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
 *  @param[in] inputs The InputMap instance we are populating.
 *  @param[in] args The inputs we are adding to @p inputs.
 *  @param[in] <anonymous> An object used purely for it's template non-type
 *             parameters.
 *
 *  @return @p inputs after populating it from @p args.
 */
template<typename PT, typename ArgsType, std::size_t... I>
auto py_wrap_inputs_(type::input_map& inputs, ArgsType&& args,
                     std::index_sequence<I...>) {
    using pluginplay::python::PythonWrapper;
    return PT::wrap_inputs(inputs,
                           any::make_any_field<PythonWrapper>(args[I])...);
}

} // namespace detail_

/** @brief Used in the Python bindings of property types.
 *
 *  This method forms the guts of the Python bindings for the `wrap_inputs`
 *  method. It will unpack the variadic @p args parameter so that the i-th
 *  argument in @p args is used as the i-th input.
 *
 *  @note `py_wrap_inputs` is templated on @p ArgsType so that it will compile
 *        even if pybind11 support is not enabled. If pybind11 support is not
 *        enabled, then attempting to call this function will result in a
 *        runtime error.
 *
 *  @tparam PT The type of the property type we are wrapping inputs for.
 *  @tparam ArgsType The type of the object representing Python's "args"
 *          concept. @p ArgsType is assumed to be pybind11::args.
 *
 *  @param[in] inputs The `type::input_map` object we are filling in.
 *  @param[in] args The inputs passed in to the property type from Python.
 *
 *  @return @p inputs after filling it in based on @p args.
 */
template<typename PT, typename ArgsType>
auto py_wrap_inputs(type::input_map& inputs, ArgsType&& args) {
    using input_tuple_type = decltype(PT::inputs());
    constexpr auto n       = input_tuple_type::size();
    constexpr auto indices = std::make_index_sequence<n>();
    return detail_::py_wrap_inputs_<PT>(inputs, std::forward<ArgsType>(args),
                                        indices);
}

} // namespace pluginplay::python
