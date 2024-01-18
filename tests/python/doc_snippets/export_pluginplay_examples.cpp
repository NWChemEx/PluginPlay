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

#include "../../cxx/doc_snippets/electric_field.hpp"
#include "../../cxx/doc_snippets/force.hpp"
#include "../../cxx/doc_snippets/load_modules.hpp"
#include "../../cxx/doc_snippets/point_charge.hpp"
#include <pluginplay/plugin/plugin.hpp>
#include <pybind11/operators.h>


namespace pluginplay_examples {

EXPORT_PLUGIN(pluginplay_examples, m) {
    pybind11::class_<PointCharge>(m, "PointCharge")
      .def(pybind11::init<>())
      .def(pybind11::init<double, Point>())
      .def_readwrite("m_charge", &PointCharge::m_charge)
      .def_readwrite("m_r", &PointCharge::m_r)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self < pybind11::self);

    EXPORT_PROPERTY_TYPE(ElectricField, m);
    EXPORT_PROPERTY_TYPE(Force, m);
}

} // namespace pluginplay_examples
