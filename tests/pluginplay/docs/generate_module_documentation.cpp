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

#include <filesystem>
#include <pluginplay/printing/document_modules.hpp>

// TODO: Replace with the header containing your ``load_modules`` definition
#include "load_modules.hpp"

namespace fs = std::filesystem;

namespace pluginplay_examples {

int main(int argc, char** argv) {
    // Need docs_path
    // This example assumes it is the first command line argument, but this
    // should be validated in actual code.
    fs::path docs_path(argv[1]);

    // Set up module manager
    pluginplay::ModuleManager mm;

    // TODO: Load your modules
    pluginplay_examples::load_modules(mm);

    // Generate module documentation at the given path
    pluginplay::printing::document_modules(mm, docs_path);

    return 0;
}

} // namespace pluginplay_examples
