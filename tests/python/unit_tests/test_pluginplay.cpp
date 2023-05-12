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

#include "any/test_any.hpp"
#include "fields/test_fields.hpp"
#include "python/test_python.hpp"
#include <pybind11/pybind11.h>

namespace test_pluginplay {

PYBIND11_MODULE(py_test_pluginplay, m) {
    test_any(m);
    test_fields(m);
    test_python_wrapper(m);
}

} // namespace test_pluginplay
