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
// #include <pybind11/stl.h>
// #include <pybind11/operators.h>

// #include "point_charge.hpp"

// namespace py = pybind11;
// using namespace pluginplay_examples;

// PYBIND11_MODULE(pointcharge, m) {
//     m.doc() = "Python bindings for the PointCharge class";

//     py::class_<PointCharge>(m, "PointCharge")
//         .def(py::init<double, Point&>())
//         .def_readwrite("m_charge", &PointCharge::m_charge)
//         .def_readwrite("m_r", &PointCharge::m_r)
//         .def("__eq__", &PointCharge::operator==, py::is_operator())
//         .def("__lt__", &PointCharge::operator<, py::is_operator())
//         .def("__repr__",
//             [](const PointCharge &p) {
//                 return "<PointCharge m_charge=" + std::to_string(p.m_charge)
//                 + " m_r=["
//                     + std::to_string(p.m_r[0]) + ", "
//                     + std::to_string(p.m_r[1]) + ", "
//                     + std::to_string(p.m_r[2]) + "]>";
//             }
//         );
// }
