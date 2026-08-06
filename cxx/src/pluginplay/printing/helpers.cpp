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

#include "helpers.hpp"

namespace pluginplay::printing {

void generate_index_file(std::ostream& os,
                         const std::vector<std::string>& toc) {
    // Generate index.rst text
    std::string index = "***********\nModules API\n***********\n\n"
                        ".. toctree::\n   :maxdepth: 1\n";

    // Add file names to the toctree
    for(std::string file : toc) { index += std::string("\n   ") + file; }

    // Write to the std::ostream
    os << index;
}

std::string replace_invalid_chars(const std::string& str,
                                  const std::string& invalid_chars,
                                  const char& replacement) {
    std::string valid_str = str;

    // Loop over list of invalid chars and replace them
    for(auto invalid_char : invalid_chars) {
        std::replace(valid_str.begin(), valid_str.end(), invalid_char,
                     replacement);
    }

    return valid_str;
}

} // namespace pluginplay::printing
