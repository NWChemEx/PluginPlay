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

namespace test_pluginplay {

void test_py_type_info(pybind11::module_& m);
void test_python_wrapper(pybind11::module_& m);

inline void test_python(pybind11::module_& m) {
    test_py_type_info(m);
    test_python_wrapper(m);
}

} // namespace test_pluginplay
