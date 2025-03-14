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

#pragma once
#include <pluginplay/module_manager/module_manager_class.hpp>
#include <sstream>

namespace pluginplay::printing {

std::stringstream create_mermaid_graph(const pluginplay::ModuleManager& mm);

void print_submods(const std::string module_key,
                   const pluginplay::ModuleManager& mm, std::stringstream& ss,
                   std::string& module_code);
} // namespace pluginplay::printing
