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

#include "../export_pluginplay.hpp"
#include <pluginplay/property_type/python_only_property_type.hpp>

namespace pluginplay::python {

namespace {

pybind11::tuple py_unwrap(const std::vector<PythonWrapper>& unwrapped) {
    pybind11::tuple rv(unwrapped.size());
    for(std::size_t i = 0; i < unwrapped.size(); ++i)
        rv[i] =
          const_cast<PythonWrapper&>(unwrapped[i]).unwrap<pybind11::object>();
    return rv;
}

} // namespace

void export_python_only_property_type(py_module_reference m) {
    py_class_type<PythonOnlyPropertyType>(m, "PythonOnlyPropertyType")
      .def(pybind11::init<std::string>(), pybind11::arg("name"))
      .def("name", &PythonOnlyPropertyType::name)
      .def("declare_input", &PythonOnlyPropertyType::declare_input,
           pybind11::return_value_policy::reference)
      .def("declare_result", &PythonOnlyPropertyType::declare_result,
           pybind11::return_value_policy::reference)
      .def("inputs", &PythonOnlyPropertyType::inputs)
      .def("results", &PythonOnlyPropertyType::results)
      .def("input_order", &PythonOnlyPropertyType::input_order)
      .def("result_order", &PythonOnlyPropertyType::result_order)
      .def("wrap_inputs",
           [](PythonOnlyPropertyType& self, type::input_map& inputs,
              pybind11::args args) {
               return self.wrap_inputs(inputs, std::move(args));
           })
      .def("wrap_results",
           [](PythonOnlyPropertyType& self, type::result_map& results,
              pybind11::args args) {
               return self.wrap_results(results, std::move(args));
           })
      .def("unwrap_inputs",
           [](PythonOnlyPropertyType& self, const type::input_map& inputs) {
               return py_unwrap(self.unwrap_inputs(inputs));
           })
      .def("unwrap_results",
           [](PythonOnlyPropertyType& self, const type::result_map& results) {
               return py_unwrap(self.unwrap_results(results));
           });
}

} // namespace pluginplay::python
