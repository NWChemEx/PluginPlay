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
#include <pluginplay/python/python.hpp>
#include <pluginplay/types.hpp>
#include <utility>

// ------------------------- Helper Macros ------------------------------------

/** @brief Declares a new PropertyType
 *
 *  This macro will declare a derived class @p prop_type which inherits from
 *  PropertyType and forwarded declares all necessary hooks.
 *
 *  @param[in] prop_type The name to use for the resulting property type.
 *
 */
#define DECLARE_PROPERTY_TYPE(prop_type)                     \
    struct prop_type : pluginplay::PropertyType<prop_type> { \
        auto inputs_();                                      \
        auto results_();                                     \
    }

/** @brief Starts the definition of a property types' inputs hook.
 *
 *  The inputs hook of a class derived from PropertyTypes establishes the fields
 *  that the module recognizes as input. The hook is implemented as a member
 *  function of the @p prop_type class with the macro taking care of declaring
 *  the hook's signature so that all you need to do is declare the function's
 *  body. If @p prop_type is a derived class only add inputs specific to
 *  @p prop_type; inputs from base property types will be added automatically.
 *
 *  @param[in] prop_type The name of the class defining the property type.
 */
#define PROPERTY_TYPE_INPUTS(prop_type) inline auto prop_type::inputs_()

/** @brief Starts the definition of a property types' results hook.
 *
 *  The results hook of a class derived from PropertyTypes establishes the
 *  fields that the module returns. The hook is implemented as a member
 *  function of the @p prop_type class with the macro taking care of declaring
 *  the hook's signature so that all you need to do is declare the function's
 *  body. If @p prop_type is a derived property type only return the results
 *  specific to @p prop_type as results from base property types will be added
 *  automatically.
 *
 *  @param[in] prop_type The name of the class defining the property type.
 */
#define PROPERTY_TYPE_RESULTS(prop_type) inline auto prop_type::results_()

/** @brief Declares a property type which is derived from another property type.
 *
 *  This macro operates similarly to DECLARE_PROPERTY_TYPE except that also
 *  takes the type of the base property type. The inputs and results of the
 *  resulting property type should be specified using PROPERTY_TYPE_INPUTS and
 *  PROPERTY_TYPE_RESULTS respectively. The implementations of
 *
 *  @param[in] prop_type The name of the derived property type.
 *  @param[in] base_type The type of the property type @p prop_type derives
 *                       from. @p base_type must have been declared previously
 *                       and the declaration must be in scope.
 */
#define DECLARE_DERIVED_PROPERTY_TYPE(prop_type, base_type)             \
    struct prop_type : pluginplay::PropertyType<prop_type, base_type> { \
        auto inputs_();                                                 \
        auto results_();                                                \
    }

/** @brief Declares a PropertyType that is templated on one or more types
 *
 *  This macro wraps the process of declaring a templated property type. The
 *  property type will still need to be instantiated. This macro is the
 *  templated version of `DECLARE_PROPERTY_TYPE`.
 *
 *  @param[in] prop_type The name of the property type
 *  @param[in] ...       The names of the template type parameters
 *
 */
#define DECLARE_TEMPLATED_PROPERTY_TYPE(prop_type, ...)                   \
    struct prop_type : pluginplay::PropertyType<prop_type<__VA_ARGS__>> { \
        auto inputs_();                                                   \
        auto results_();                                                  \
    }

/** @brief Declares a PropertyType that derives from a property type and
 *         is templated on one or more types
 *
 *  This macro wraps the process of declaring a templated property type. The
 *  property type will still need to be instantiated. This macro is the
 *  templated version of `DECLARE_DERIVED_PROPERTY_TYPE`. Inputs and results
 *  should be added by using `TEMPLATED_PROPERTY_TYPE_INPUTS` and
 *  `TEMPLATED_PROPERTY_TYPE_RESULTS` respectively.
 *
 *  @param[in] prop_type The name of the property type
 *  @param[in] base_type The name of the property type that @p prop_type
 *                       derives from.
 *  @param[in] ...       The names of the template type parameters
 *
 */
#define DECLARE_DERIVED_TEMPLATED_PROPERTY_TYPE(prop_type, base_type, ...) \
    struct prop_type                                                       \
      : pluginplay::PropertyType<prop_type<__VA_ARGS__>, base_type> {      \
        auto inputs_();                                                    \
        auto results_();                                                   \
    }

/** @brief Defines the inputs_() method of a templated PropertyType
 *
 *  This macro is the templated version of `PROPERTY_TYPE_INPUTS`. It wraps the
 *  signature part of the inputs_() method definition.
 *
 *  @param[in] prop_type The name of the PropertyType whose inputs_() method is
 *                       being defined.
 *  @param[in] ...       The names of the template type parameters.
 */
#define TEMPLATED_PROPERTY_TYPE_INPUTS(prop_type, ...) \
    inline auto prop_type<__VA_ARGS__>::inputs_()

/** @brief Defines the results_() method of a templated PropertyType
 *
 *  This macro is the templated version of `PROPERTY_TYPE_RESULTS`. It wraps the
 *  signature part of the results_() method definition.
 *
 *  @param[in] prop_type The name of the PropertyType whose results_() method is
 *                       being defined.
 *  @param[in] ...       The names of the template type parameters.
 */
#define TEMPLATED_PROPERTY_TYPE_RESULTS(prop_type, ...) \
    inline auto prop_type<__VA_ARGS__>::results_()

#define EXPORT_PROPERTY_TYPE(prop_type, m)                                  \
    pybind11::class_<prop_type>(m, #prop_type)                              \
      .def(pybind11::init<>())                                              \
      .def("type",                                                          \
           [](prop_type&) {                                                 \
               return pluginplay::python::PyTypeInfo(typeid(prop_type));    \
           })                                                               \
      .def("inputs",                                                        \
           [](prop_type& pt) {                                              \
               auto rvs = pt.inputs();                                      \
               return pluginplay::type::input_map(rvs.begin(), rvs.end());  \
           })                                                               \
      .def("results",                                                       \
           [](prop_type& pt) {                                              \
               auto rvs = pt.results();                                     \
               return pluginplay::type::result_map(rvs.begin(), rvs.end()); \
           })                                                               \
      .def("unwrap_inputs",                                                 \
           [](prop_type& pt, pluginplay::type::input_map& inputs) {         \
               return pt.unwrap_inputs(inputs);                             \
           })                                                               \
      .def("wrap_inputs",                                                   \
           [](prop_type& pt, pluginplay::type::input_map& inputs,           \
              pybind11::args args) {                                        \
               return pluginplay::python::py_wrap_inputs<prop_type>(        \
                 inputs, std::move(args));                                  \
           })                                                               \
      .def("unwrap_results",                                                \
           [](prop_type& pt, pluginplay::type::result_map& results) {       \
               return pt.unwrap_results(results);                           \
           })                                                               \
      .def("wrap_results",                                                  \
           [](prop_type& pt, pluginplay::type::result_map& results,         \
              pybind11::args args) {                                        \
               return pluginplay::python::py_wrap_results<prop_type>(       \
                 results, std::move(args));                                 \
           })
