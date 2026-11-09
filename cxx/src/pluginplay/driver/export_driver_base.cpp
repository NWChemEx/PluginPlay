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

#include "export_driver.hpp"
#include "py_driver_base.hpp"
#include <pluginplay/any/any.hpp>
#include <pluginplay/python/python_wrapper.hpp>

namespace pluginplay {

/// Exposes the protected members of DriverBase to the Python bindings
class DriverPublicist : public DriverBase {
public:
    using DriverBase::driver_input;
    using DriverBase::driver_submodule;
    using DriverBase::when;
    using DriverBase::wire_memoization;
};

void export_driver_base(py_module_reference m) {
    using python::PythonWrapper;
    using qualified_key = QualifiedKey;
    using reference     = py::return_value_policy;

    py_class_type<DriverInput>(m, "DriverInput")
      .def(
        "set_default",
        [](DriverInput& self, py::object value) -> DriverInput& {
            return self.set_default(any::make_any_field<PythonWrapper>(value));
        },
        reference::reference)
      .def("set_description", &DriverInput::set_description,
           reference::reference)
      .def("make_optional", &DriverInput::make_optional, reference::reference)
      .def("make_required", &DriverInput::make_required, reference::reference)
      .def("make_opaque", &DriverInput::make_opaque, reference::reference)
      .def("make_transparent", &DriverInput::make_transparent,
           reference::reference)
      .def("set_target", &DriverInput::set_target, reference::reference)
      .def("set_required_target", &DriverInput::set_required_target,
           reference::reference)
      .def("input", &DriverInput::input, reference::reference_internal);

    py_class_type<DriverSubmodule>(m, "DriverSubmodule")
      .def("set_description", &DriverSubmodule::set_description,
           reference::reference)
      .def("set_default", &DriverSubmodule::set_default, reference::reference)
      .def("set_target", &DriverSubmodule::set_target, reference::reference)
      .def("set_required_target", &DriverSubmodule::set_required_target,
           reference::reference)
      .def("submodule", &DriverSubmodule::submodule,
           reference::reference_internal);

    using pointer_type = std::shared_ptr<DriverBase>;
    py_class_type<DriverBase, PyDriverBase, ModuleBase, pointer_type>(
      m, "DriverBase")
      .def(py::init<>())
      .def(
        "add_input",
        [](DriverBase& self, std::string key) {
            self.inputs()[key].set_type<PythonWrapper>();
            return (self.*(&DriverPublicist::driver_input))(key);
        },
        py::keep_alive<0, 1>())
      .def(
        "add_submodule",
        [](DriverBase& self, py::object pt, std::string key) {
            auto py_sr = py::cast(SubmoduleRequest());
            py_sr.attr("set_type")(pt);
            self.submods().emplace(key, py_sr.cast<SubmoduleRequest>());
            return (self.*(&DriverPublicist::driver_submodule))(key);
        },
        py::keep_alive<0, 1>())
      .def(
        "when",
        [](DriverBase& self, std::string key,
           py::object value) -> ConditionalWiring& {
            auto when = &DriverPublicist::template when<type::any>;
            return (self.*when)(std::move(key),
                                any::make_any_field<PythonWrapper>(value));
        },
        reference::reference_internal)
      .def(
        "wire_memoization",
        [](DriverBase& self, bool on, std::vector<qualified_key> except) {
            (self.*(&DriverPublicist::wire_memoization))(on, std::move(except));
        },
        py::arg("on"), py::arg("except") = std::vector<qualified_key>{})
      .def("call_graph_request", &DriverBase::call_graph_request,
           reference::reference_internal);
}

} // namespace pluginplay
