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
