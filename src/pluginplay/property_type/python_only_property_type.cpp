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

#include <pluginplay/property_type/python_only_property_type.hpp>

namespace pluginplay::python {

ModuleInput& PythonOnlyPropertyType::declare_input(const type::key& key) {
    auto& in = m_inputs_[key];
    in.set_type<PythonWrapper>();
    m_input_order_.push_back(key);
    return in;
}

ModuleResult& PythonOnlyPropertyType::declare_result(const type::key& key) {
    auto& r = m_results_[key];
    r.set_type<PythonWrapper>();
    m_result_order_.push_back(key);
    return r;
}

std::vector<PythonWrapper> PythonOnlyPropertyType::unwrap_inputs(
  const type::input_map& im) const {
    std::vector<PythonWrapper> rv;
    rv.reserve(m_input_order_.size());
    for(const auto& key : m_input_order_)
        rv.push_back(im.at(key).value<PythonWrapper>());
    return rv;
}

std::vector<PythonWrapper> PythonOnlyPropertyType::unwrap_results(
  const type::result_map& rm) const {
    std::vector<PythonWrapper> rv;
    rv.reserve(m_result_order_.size());
    for(const auto& key : m_result_order_)
        rv.push_back(rm.at(key).value<PythonWrapper>());
    return rv;
}

} // namespace pluginplay::python
