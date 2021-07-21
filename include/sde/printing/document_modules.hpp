#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "sde/module_manager.hpp"
#include "sde/printing/document_module.hpp"

namespace fs = std::filesystem;

namespace sde::printing {

// TODO: Determine a final place for these functions. This is probably not
//       the file for them.
int generate_index_file(fs::path location, std::vector<std::string> toc);
std::string replace_invalid_chars(const std::string str,
				  const std::string invalid_chars,
				  const char replacement);
int write_file(std::string file, std::string text);

    
/** @brief Generate documentation for all modules in a given module manager.
 *
 * Use @ref document_module to generate documentation for each module in a
 * given module manager and output them to a given path. This will also generate
 * an index file for the modules to be imported into a documentation generator
 * like Sphinx.
 * 
 * @param[in] mm Module manager with modules loaded into it.
 * @param[in] docs_path Path to the directory where documentation is output.
 *
 * @returns TBD
 */
int document_modules(sde::ModuleManager mm, fs::path docs_path);

} // namespace sde::printing
