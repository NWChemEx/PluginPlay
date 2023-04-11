/*
 * Copyright 2022 NWChemEx-Project
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
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "pluginplay/module_manager.hpp"
#include "pluginplay/printing/document_module.hpp"

namespace pluginplay::printing {

/** @brief Generate documentation for all modules in a given module manager.
 *
 * Use @ref document_module to generate documentation for each module in a
 * given module manager and output them to a given path. This will also generate
 * an index file for the modules to be imported into a documentation generator
 * like Sphinx.
 *
 * @param[in] mm Module manager with modules loaded into it.
 * @param[in] docs_path Path to the directory where documentation is output.
 */
void document_modules(pluginplay::ModuleManager& mm,
                      const std::filesystem::path& docs_path);

} // namespace pluginplay::printing
