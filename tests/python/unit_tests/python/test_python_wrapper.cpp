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

#include <pluginplay/python/python_wrapper.hpp>
#include <pybind11/pybind11.h>
#include <vector>

namespace test_pluginplay {

/// C++ functions used by the test_python_wrapper unit test suite
void test_python_wrapper(pybind11::module_& m) {
    auto m_pywrap = m.def_submodule("test_python_wrapper");

    using pluginplay::python::PythonWrapper;
    m_pywrap.def("cxx_unwrap_list", [](PythonWrapper& w) {
        bool has_value = w.has_value();
        auto vec_copy  = w.unwrap<std::vector<int>>();
        bool are_equal = (vec_copy == std::vector<int>{1, 2, 3});
        return has_value && are_equal;
    });
    m_pywrap.def("cxx_unwrap_dict", [](PythonWrapper& w) {
        bool has_value = w.has_value();
        auto map_copy  = w.unwrap<std::map<std::string, int>>();
        std::map<std::string, int> corr;
        corr["hello"]  = 42;
        corr["world"]  = 123;
        bool are_equal = (map_copy == corr);
        return has_value && are_equal;
    });
}
} // namespace test_pluginplay
