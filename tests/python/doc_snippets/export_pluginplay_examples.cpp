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


// #include <pybind11/pybind11.h>
// // #include <pybind11/stl.h>
// // #include <pybind11/stl_bind.h>

// // #include "point_charge.hpp"
// // #include "templated_coulombs_law.cpp"
// // #include "screened_coulombs_law.cpp"
// // #include "force.cpp"
// // #include "modules.hpp"
// // #include <pluginplay/pluginplay.hpp>
// #include <pluginplay/plugin/plugin.hpp>

// namespace py = pybind11;

// PYBIND11_MODULE(pluginplay_examples, m) {
//     m.doc() = "pybind11 bindings for pluginplay_examples";

//     // Binding for Point
//     //py::bind_vector<std::array<double, 3>>(m, "Point");

//     // Binding for PointCharge
//     py::class_<pluginplay_examples::PointCharge>(m, "PointCharge")
//       .def(py::init<>())
//     .def_readwrite("charge", &pluginplay_examples::PointCharge::m_charge,
//                    "Charge of the point")
//       .def_readwrite("position", &pluginplay_examples::PointCharge::m_r,
//                      "Position of the point");

//     EXPORT_PLUGIN(scf, m) {}

//     // py::class_<pluginplay_examples::CoulombsLaw, pluginplay::ModuleBase>(
//     //   m, "CoulombsLaw")
//     //   .def(py::init<>());
//     // // .def("inputs", static_cast<void
//     // // (pluginplay_examples::CoulombsLaw::*)()>(&pluginplay_examples::CoulombsLaw::inputs))
//     // // .def("results", &pluginplay_examples::CoulombsLaw::results);

//     // // Revised Binding for TemplatedCoulombsLaw
//     // py::class_<pluginplay_examples::TemplatedCoulombsLaw<float>,
//     //            pluginplay::ModuleBase>(m, "TemplatedCoulombsLawFloat")
//     //   .def(py::init<>());
//     // // .def("inputs",
//     // // &pluginplay_examples::TemplatedCoulombsLaw<float>::inputs)
//     // // .def("results",
//     // // &pluginplay_examples::TemplatedCoulombsLaw<float>::results);

//     // // Revised Binding for ScreenedCoulombsLaw
//     // py::class_<pluginplay_examples::ScreenedCoulombsLaw,
//     //            pluginplay::ModuleBase>(m, "ScreenedCoulombsLaw")
//     //   .def(py::init<>());
//     // // .def("inputs", &pluginplay_examples::ScreenedCoulombsLaw::inputs)
//     // // .def("results", &pluginplay_examples::ScreenedCoulombsLaw::results);

//     // // Revised Binding for ClassicalForce
//     // py::class_<pluginplay_examples::ClassicalForce, pluginplay::ModuleBase>(
//     //   m, "ClassicalForce")
//     //   .def(py::init<>());
//     // // .def("inputs", &pluginplay_examples::ClassicalForce::inputs)
//     // // .def("results", &pluginplay_examples::ClassicalForce::results);
// }

#include <pluginplay/plugin/plugin.hpp>
#include <pybind11/pybind11.h>
#include "../../cxx/doc_snippets/load_modules.hpp"
#include <export_electric_field.hpp>


namespace pluginplay_examples {
//EXPORT_PROPERTY_TYPE(ElectricField, m);

EXPORT_PLUGIN(pluginplay_examples, m){};
// pybind11::class_<DerivedClass, BaseClass>(m, "DerivedClass");

} // namespace pluginplay_examples