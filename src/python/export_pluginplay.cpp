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

#include "any/export_any.hpp"
#include "fields/export_fields.hpp"
#include "python/export_python.hpp"
#include <pybind11/pybind11.h>

namespace pluginplay {

PYBIND11_MODULE(pluginplay, m) {
    any::export_any(m);
    export_fields(m);
    python::export_python(m);
    export_module(m);
    export_module_base(m);
    export_module_manager(m);
    export_submodule_request(m);
}

} // namespace pluginplay
