#include <sde/printing/document_modules.hpp>
#include <filesystem>

// TODO: Replace with the header containing your ``load_modules`` definition
#include "load_modules.hpp"

namespace fs = std::filesystem;

namespace sde_examples {
    
int main(int argc, char** argv) {
    // Need docs_path
    // This example assumes it is the first command line argument, but this
    // should be validated in actual code.
    fs::path docs_path(argv[1]);
    
    // Set up module manager
    sde::ModuleManager mm;
    
    // TODO: Load your modules
    sde_examples::load_modules(mm);
    
    // Generate module documentation at the given path
    sde::printing::document_modules(mm, docs_path);

    return 0;
}

} // namespace sde_examples
