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

#include "../../cxx/doc_snippets/load_modules.hpp"
#include <export_electric_field.hpp>
#include <pluginplay/plugin/plugin.hpp>
#include <pybind11/pybind11.h>

namespace pluginplay_examples {

EXPORT_PLUGIN(py_pluginplay_examples, m){};

} // namespace pluginplay_examples
