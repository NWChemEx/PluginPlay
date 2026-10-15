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

#include "../catch.hpp"
#include "../examples/writing_a_module.hpp"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <pluginplay/plugin/plugin.hpp>
#include <pluginplay/printing/document_modules.hpp>
#include <sstream>

namespace {

DECLARE_PLUGIN(DocumentedPlugin);

PLUGIN_CTOR(DocumentedPlugin) {
    set_name("documented_plugin");
    add_module<Rectangle>("Rectangle");
    add_module<Prism>("Prism");
}

// Number of entries in the directory dir
auto n_entries(const std::filesystem::path& dir) {
    return std::distance(std::filesystem::directory_iterator(dir),
                         std::filesystem::directory_iterator{});
}

} // namespace

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

    SECTION("Own modules and a plugin") {
        mm.add_module<Rectangle>("Own Rectangle");
        mm.load_plugin<DocumentedPlugin>();

        pluginplay::printing::document_modules(mm, docs_path);

        // index.rst, Own_Rectangle.rst, and the plugin's directory
        const auto plugin_path = docs_path / "documented_plugin";
        REQUIRE(n_entries(docs_path) == 3);
        REQUIRE(std::filesystem::exists(docs_path / "Own_Rectangle.rst"));
        REQUIRE(std::filesystem::is_directory(plugin_path));
        REQUIRE(n_entries(plugin_path) == 2);
        REQUIRE(std::filesystem::exists(plugin_path / "Rectangle.rst"));
        REQUIRE(std::filesystem::exists(plugin_path / "Prism.rst"));

        // The index points into the plugin's directory
        std::ifstream index(docs_path / "index.rst");
        std::stringstream contents;
        contents << index.rdbuf();
        REQUIRE(contents.str().find("documented_plugin/Prism") !=
                std::string::npos);
    }
}
