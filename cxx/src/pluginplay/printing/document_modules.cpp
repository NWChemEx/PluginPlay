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
#include "pluginplay/printing/document_modules.hpp"

namespace pluginplay::printing {

void document_modules(pluginplay::ModuleManager& mm,
                      const std::filesystem::path& docs_path) {
    const std::string INVALID_CHARS = "<>:\"|?*\\/ ";
    const char REPLACEMENT_CHAR     = '_';

    std::ofstream fout;

    // Throw exception on badbit being set
    fout.exceptions(std::ofstream::badbit);

    // List of files for index file
    std::vector<std::string> toc;

    // Loop over the maps, then the key-value pairs of module names and module
    // objects for docs. The ModuleManager's own modules go in docs_path; each
    // plugin's modules go in a subdirectory named after the plugin. Plugin
    // names are identifiers (see Plugin::is_valid_name), so they are already
    // safe, distinct directory names.
    for(const auto& plugin : mm.plugin_names()) {
        std::filesystem::path dir = docs_path;
        std::string toc_prefix;
        if(!plugin.empty()) {
            dir /= plugin;
            std::filesystem::create_directories(dir);
            toc_prefix = plugin + "/";
        }

        for(const auto& [name, module] : mm.get_modules(plugin)) {
            std::string clean_name =
              replace_invalid_chars(name, INVALID_CHARS, REPLACEMENT_CHAR);

            // Gather file name in the toc for the index file
            toc.push_back(toc_prefix + clean_name);

            // Generate documentation
            std::string docs = document_module(name, *module);

            // Create the file path
            std::filesystem::path file_path = dir;
            file_path /= clean_name + ".rst";

            fout.open(file_path);

            // Write to documentation file
            fout << docs;

            fout.close();
        }
    }

    // Create index file
    std::filesystem::path file_path = docs_path;
    file_path /= "index.rst";

    fout.open(file_path);

    generate_index_file(fout, toc);

    fout.close();
}

} // namespace pluginplay::printing
