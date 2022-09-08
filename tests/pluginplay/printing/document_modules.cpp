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

#include <catch2/catch.hpp>
#include <filesystem>
#include <iterator>
#include <pluginplay/printing/document_modules.hpp>

#include "../docs/load_modules.hpp"

TEST_CASE("document_modules") {
    // Need a docs path
    const auto root_dir = std::filesystem::temp_directory_path();
    const std::filesystem::path docs_dir("docs");
    auto docs_path = root_dir / docs_dir;

    std::filesystem::remove_all(docs_path);

    // Create the directory
    std::filesystem::create_directory(docs_path);

    // Verify the directory exists
    REQUIRE(std::filesystem::exists(docs_path));
    REQUIRE(std::filesystem::is_directory(docs_path));
    REQUIRE(std::distance(std::filesystem::directory_iterator(docs_path),
                          std::filesystem::directory_iterator{}) ==
            0); // Empty directory

    // Set up module manager
    pluginplay::ModuleManager mm;

    SECTION("Empty Module Manager") {
        int correct = 1; // for index.html

        // Generate module documentation at the given path
        pluginplay::printing::document_modules(mm, docs_path);

        REQUIRE(std::distance(std::filesystem::directory_iterator(docs_path),
                              std::filesystem::directory_iterator{}) ==
                correct);
    }

    SECTION("Modules Loaded") {
        // Load your modules
        pluginplay_examples::load_modules(mm);

        // Generate module documentation at the given path
        pluginplay::printing::document_modules(mm, docs_path);

        REQUIRE(std::distance(std::filesystem::directory_iterator(docs_path),
                              std::filesystem::directory_iterator{}) ==
                mm.size() + 1); // +1 for index.rst
    }
}
