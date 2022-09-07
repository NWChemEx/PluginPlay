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
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <vector>

namespace pluginplay::printing {

/** @brief Generate index.rst with table of contents for documentation files.
 *
 * Generates text for an `index.rst` file with a table of contents that includes
 * all of the generated module documentation files.
 *
 * @param[in] os std::ostream to write the text to.
 * @param[in] toc Vector of file names for documentation files without ".rst".
 */
void generate_index_file(std::ostream& os, const std::vector<std::string>& toc);

/** @brief Replace invalid characters in a string.
 *
 * Replaces the provided invalid characters with the provided replacement
 * characters.
 *
 * @param[in] str String to replace characters in.
 * @param[in] invalid_chars List of invalid characters.
 * @param[in] replacement Replacement character for invalid charactes.
 *
 * @returns String with invalid characters replaced.
 */
std::string replace_invalid_chars(const std::string& str,
                                  const std::string& invalid_chars,
                                  const char& replacement);

} // namespace pluginplay::printing
