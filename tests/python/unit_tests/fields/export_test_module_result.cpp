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

#include "test_fields.hpp"
#include <pluginplay/fields/module_result.hpp>

using namespace pluginplay;

namespace test_pluginplay {

void test_module_result(pybind11::module_& m) {
    auto m_test = m.def_submodule("test_module_result");

    m_test.def("get_r_float", []() {
        ModuleResult r;
        r.set_type<double>();
        return r;
    });

    m_test.def("get_r_list", []() {
        ModuleResult r;
        r.set_type<std::vector<int>>();
        return r;
    });

    m_test.def("get_filled_r_list", []() {
        ModuleResult r;
        r.set_type<std::vector<int>>();
        r.change(std::vector<int>{1, 2, 3});
        return r;
    });

    m_test.def("unwrap_python_list", [](ModuleResult& r) {
        const auto value = r.value<std::vector<int>>();
        return value == std::vector<int>{1, 2, 3};
    });
}

} // namespace test_pluginplay
