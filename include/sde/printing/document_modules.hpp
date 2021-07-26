#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "sde/module_manager.hpp"
#include "sde/printing/document_module.hpp"

namespace sde::printing {

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
void document_modules(sde::ModuleManager& mm,
                      const std::filesystem::path& docs_path);

} // namespace sde::printing
