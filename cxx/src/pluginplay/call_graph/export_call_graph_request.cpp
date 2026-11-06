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

#include "export_call_graph.hpp"
#include <pluginplay/any/any.hpp>
#include <pluginplay/call_graph/call_graph.hpp>
#include <pluginplay/python/python_wrapper.hpp>

namespace pluginplay {
namespace {

/// Wraps a Python object for a wiring request
type::any wrap(py::object value) {
    return any::make_any_field<python::PythonWrapper>(std::move(value));
}

/// Unwraps a value of a wiring request into a Python object
py::object unwrap(const type::any& value) {
    return any::any_cast<python::PythonWrapper>(value)
      .template unwrap<py::object>();
}

} // namespace

void export_call_graph_request(py_module_reference m) {
    using qualified_key = QualifiedKey;
    using key_type      = type::key;
    using reference     = py::return_value_policy;

    py_class_type<WiringRequest> wiring_request(m, "WiringRequest");
    py::enum_<WiringRequest::kind_type>(wiring_request, "kind_type")
      .value("set_input", WiringRequest::kind_type::set_input)
      .value("set_submod", WiringRequest::kind_type::set_submod)
      .value("forward_input", WiringRequest::kind_type::forward_input)
      .value("forward_submod", WiringRequest::kind_type::forward_submod);
    wiring_request
      .def("required", &WiringRequest::required, py::arg("value") = true,
           reference::reference)
      .def("is_required", &WiringRequest::is_required)
      .def("kind", &WiringRequest::kind)
      .def("sets_input", &WiringRequest::sets_input)
      .def("is_forward", &WiringRequest::is_forward)
      .def("module_key", &WiringRequest::module_key)
      .def("target_key", &WiringRequest::target_key)
      .def("value",
           [](const WiringRequest& self) { return unwrap(self.value()); })
      .def("submod_key", &WiringRequest::submod_key)
      .def("source_key", &WiringRequest::source_key)
      .def("to_string", &WiringRequest::to_string)
      .def("__str__", &WiringRequest::to_string)
      .def(py::self == py::self)
      .def(py::self != py::self);

    using branch_type = ConditionalWiring::Branch;
    py_class_type<ConditionalWiring> chain(m, "ConditionalWiring");
    py_class_type<branch_type>(chain, "Branch")
      .def("has_condition", &branch_type::has_condition)
      .def("input_key", &branch_type::input_key)
      .def("value",
           [](const branch_type& self) { return unwrap(self.value()); })
      .def("requests", &branch_type::requests)
      .def("is_required", &branch_type::is_required)
      .def("condition_string", &branch_type::condition_string);
    chain
      .def(
        "set_input",
        [](ConditionalWiring& self, qualified_key module_key,
           key_type input_key, py::object value) -> ConditionalWiring& {
            return self.set_input(std::move(module_key), std::move(input_key),
                                  wrap(std::move(value)));
        },
        reference::reference)
      .def("set_submod", &ConditionalWiring::set_submod, reference::reference)
      .def("forward_input", &ConditionalWiring::forward_input,
           reference::reference)
      .def("forward_submod", &ConditionalWiring::forward_submod,
           reference::reference)
      .def(
        "otherwise",
        [](ConditionalWiring& self, key_type input_key,
           py::object value) -> ConditionalWiring& {
            return self.otherwise(std::move(input_key), wrap(std::move(value)));
        },
        reference::reference)
      .def(
        "otherwise",
        [](ConditionalWiring& self) -> ConditionalWiring& {
            return self.otherwise();
        },
        reference::reference)
      .def("required", &ConditionalWiring::required, reference::reference)
      .def("branches", &ConditionalWiring::branches,
           reference::reference_internal)
      .def(py::self == py::self)
      .def(py::self != py::self);

    py_class_type<CallGraphRequest>(m, "CallGraphRequest")
      .def(py::init<>())
      .def(
        "set_input",
        [](CallGraphRequest& self, qualified_key module_key, key_type input_key,
           py::object value) -> WiringRequest& {
            return self.set_input(std::move(module_key), std::move(input_key),
                                  wrap(std::move(value)));
        },
        reference::reference_internal)
      .def("set_submod", &CallGraphRequest::set_submod,
           reference::reference_internal)
      .def("requests",
           [](const CallGraphRequest& self) {
               const auto& requests = self.requests();
               return std::vector<WiringRequest>(requests.begin(),
                                                 requests.end());
           })
      .def("conditional_wiring",
           [](const CallGraphRequest& self) {
               const auto& chains = self.conditional_wiring();
               return std::vector<ConditionalWiring>(chains.begin(),
                                                     chains.end());
           })
      .def("submod_defaults", &CallGraphRequest::submod_defaults)
      .def("empty", &CallGraphRequest::empty)
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace pluginplay
