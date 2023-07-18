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

#include "../export_pluginplay.hpp"
#include "export_printing.hpp"
#include "pluginplay/printing/document_modules.hpp"
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl/filesystem.h>

namespace pluginplay {

void export_document_modules(py_module_reference m) {
    m.def("document_modules", &pluginplay::printing::document_modules,
          "A function for exporting document modules");
}

} // namespace pluginplay
