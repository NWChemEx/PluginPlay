#include <catch2/catch.hpp>
#include <filesystem>
#include <iterator>
#include <sde/printing/document_modules.hpp>

#include "../docs/load_modules.hpp"

namespace fs = std::filesystem;

TEST_CASE("document_modules") {
    // Need a docs path
    const fs::path docs_path("./docs");

    fs::remove_all(docs_path);

    // Create the directory
    fs::create_directory(docs_path);

    // Verify the directory exists
    REQUIRE(fs::exists(docs_path));
    REQUIRE(fs::is_directory(docs_path));
    REQUIRE(std::distance(fs::directory_iterator(docs_path),
                          fs::directory_iterator{}) == 0); // Empty directory

    // Set up module manager
    sde::ModuleManager mm;

    SECTION("Empty Module Manager") {
        int correct = 1; // for index.html

        // Generate module documentation at the given path
        sde::printing::document_modules(mm, docs_path);

        REQUIRE(std::distance(fs::directory_iterator(docs_path),
                              fs::directory_iterator{}) == correct);
    }

    SECTION("Modules Loaded") {
        // Load your modules
        sde_examples::load_modules(mm);

        // Generate module documentation at the given path
        sde::printing::document_modules(mm, docs_path);

        REQUIRE(std::distance(fs::directory_iterator(docs_path),
                              fs::directory_iterator{}) ==
                mm.size() + 1); // +1 for index.rst
    }
}
