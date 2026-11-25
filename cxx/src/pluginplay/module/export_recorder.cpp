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

#include "export_module.hpp"
#include <memory>
#include <pluginplay/module/recorder.hpp>
#include <pluginplay/python/py_type_info.hpp>
#include <utility>

namespace pluginplay {
namespace {

/// Makes a Recorder satisfying the property type of the Python object @p pt
auto make_recorder(py::object pt) {
    auto rtti    = pt.attr("type")().cast<python::PyTypeInfo>().value();
    auto inputs  = pt.attr("inputs")().cast<type::input_map>();
    auto results = pt.attr("results")().cast<type::result_map>();
    return std::make_shared<Recorder>(std::move(rtti), std::move(inputs),
                                      std::move(results));
}

} // namespace

void export_recorder(py_module_reference m) {
    using pointer_type = std::shared_ptr<Recorder>;
    py_class_type<Recorder, ModuleBase, pointer_type>(m, "Recorder")
      .def(py::init(&make_recorder))
      .def("records", &Recorder::records)
      .def("clear", &Recorder::clear);
}

} // namespace pluginplay
