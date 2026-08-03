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
#include <pluginplay/fields/fields.hpp>
#include <pluginplay/python/python_wrapper.hpp>
#include <pluginplay/types.hpp>
#include <stdexcept>
#include <vector>

namespace pluginplay::python {

/** @brief Runtime (non-CRTP) analog of PropertyType<T> for property types
 *         defined entirely in Python.
 *
 *  `PropertyType<DerivedType>` relies on CRTP to give each property type a
 *  distinct C++ type (and hence a distinct RTTI). That trick doesn't help
 *  here: every field of a Python-only property type is an opaque Python
 *  object, so there's nothing for per-field C++ typing to key off, and no
 *  way to synthesize a new, distinct C++ RTTI from Python at runtime
 *  (`std::type_index` can only wrap a real `std::type_info` obtained via
 *  `typeid()`). Consequently a *single* concrete class,
 * `PythonOnlyPropertyType`,
 *  represents every Python-only property type; instances differ only in
 *  their `name()` and declared field keys, which are ordinary runtime data.
 *
 *  Because of this, all `PythonOnlyPropertyType` instances share the same
 *  RTTI (`typeid(PythonOnlyPropertyType)`). Real identity -- used to decide
 *  whether a module satisfies a *specific* Python-only property type -- is
 *  the `name()` string, checked via a set separate from the usual
 *  `ModuleBase`/`Module` RTTI-keyed property type set.
 *
 *  Every declared field is stored as a `PythonWrapper`-typed `ModuleInput`/
 *  `ModuleResult`, so once wrapped, the values flow through PluginPlay's
 *  ordinary type-erased `Module::run()` pipeline (memoization, submodule
 *  resolution, etc.) completely unchanged.
 */
class PythonOnlyPropertyType {
public:
    explicit PythonOnlyPropertyType(std::string name) :
      m_name_(std::move(name)) {}

    virtual ~PythonOnlyPropertyType() noexcept = default;

    /// The identity of this Python-only property type.
    const std::string& name() const noexcept { return m_name_; }

    /** @brief Declares a new input field named @p key.
     *
     *  Fields are always typed as PythonWrapper (i.e. an opaque Python
     *  object). The returned reference can be used to chain
     *  `.set_description(...)`/`.set_default(...)` calls, mirroring
     *  `ModuleBase::add_input`.
     */
    ModuleInput& declare_input(const type::key& key);

    /// Same as declare_input, but for a result field.
    ModuleResult& declare_result(const type::key& key);

    const type::input_map& inputs() const noexcept { return m_inputs_; }

    const type::result_map& results() const noexcept { return m_results_; }

    /// The order fields were declared in declare_input, used for positional
    /// wrap_inputs/unwrap_inputs.
    const std::vector<type::key>& input_order() const noexcept {
        return m_input_order_;
    }

    /// The order fields were declared in declare_result, used for positional
    /// wrap_results/unwrap_results.
    const std::vector<type::key>& result_order() const noexcept {
        return m_result_order_;
    }

    /** @brief Binds @p args, positionally, to @p rv's input fields.
     *
     *  @tparam ArgsType Expected to be pybind11::args, but templated (rather
     *          than including pybind11 directly) so this header stays
     *          includable without pybind11 support, mirroring
     *          pluginplay::python::py_wrap_inputs.
     */
    template<typename ArgsType>
    type::input_map wrap_inputs(type::input_map rv, ArgsType&& args) const;

    /// Same as wrap_inputs, but for result fields.
    template<typename ArgsType>
    type::result_map wrap_results(type::result_map rv, ArgsType&& args) const;

    /// Unwraps @p im's fields, in declaration order, back into Python objects.
    std::vector<PythonWrapper> unwrap_inputs(const type::input_map& im) const;

    /// Unwraps @p rm's fields, in declaration order, back into Python objects.
    std::vector<PythonWrapper> unwrap_results(const type::result_map& rm) const;

private:
    std::string m_name_;
    type::input_map m_inputs_;
    type::result_map m_results_;
    std::vector<type::key> m_input_order_;
    std::vector<type::key> m_result_order_;
};

template<typename ArgsType>
type::input_map PythonOnlyPropertyType::wrap_inputs(type::input_map rv,
                                                    ArgsType&& args) const {
    if(static_cast<std::size_t>(args.size()) != m_input_order_.size())
        throw std::runtime_error("PythonOnlyPropertyType \"" + m_name_ +
                                 "\": wrong number of positional inputs");
    for(std::size_t i = 0; i < m_input_order_.size(); ++i)
        rv.at(m_input_order_[i]).change(PythonWrapper(args[i]));
    return rv;
}

template<typename ArgsType>
type::result_map PythonOnlyPropertyType::wrap_results(type::result_map rv,
                                                      ArgsType&& args) const {
    if(static_cast<std::size_t>(args.size()) != m_result_order_.size())
        throw std::runtime_error("PythonOnlyPropertyType \"" + m_name_ +
                                 "\": wrong number of positional results");
    for(std::size_t i = 0; i < m_result_order_.size(); ++i)
        rv.at(m_result_order_[i]).change(PythonWrapper(args[i]));
    return rv;
}

} // namespace pluginplay::python
