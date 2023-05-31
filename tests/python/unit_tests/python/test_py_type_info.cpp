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

#include "test_python.hpp"
#include <pluginplay/python/py_type_info.hpp>

namespace test_pluginplay {

using PyTypeInfo = pluginplay::python::PyTypeInfo;

void test_py_type_info(pybind11::module_& m) {
    auto m_test = m.def_submodule("test_py_test_info");

    m_test.def("wraps_int", []() { return PyTypeInfo(typeid(int)); });
    m_test.def("wraps_double",
               []() { return PyTypeInfo(std::type_index(typeid(double))); });

    m_test.def("check_int", [](PyTypeInfo& pinfo) {
        PyTypeInfo corr(std::type_index(typeid(int)));
        return corr == pinfo;
    });

    m_test.def("check_double", [](PyTypeInfo& pinfo) {
        PyTypeInfo corr(std::type_index(typeid(double)));
        return corr == pinfo;
    });
}

} // namespace test_pluginplay
