#include "sde/printing/document_modules.hpp"

namespace sde::printing {

int document_modules(sde::ModuleManager mm, fs::path docs_path) {
    const std::string INVALID_CHARS = "<>:\"|?*\\/ ";
    const char REPLACEMENT_CHAR = '_';

    // List of files for index file
    std::vector<std::string> toc;
    
    // Loop over key-value pairs of module names and module objects for docs
    for (const auto& [name, module] : mm) {
	std::string clean_name = replace_invalid_chars(name,
						       INVALID_CHARS,
						       REPLACEMENT_CHAR);
	
	// Gather file name in the toc for the index file
	toc.push_back(clean_name);

	// Generate documentation
	std::string docs = document_module(name, *module);
	
	// Create the file path
	fs::path file_path = docs_path;
	file_path /= clean_name + ".rst";
	
	// Write to documentation file
	int file_result = write_file(file_path, docs);

	// Exit early if a file failed to write
	if (file_result == -1)
	    return file_result;
    }

    return generate_index_file(docs_path, toc);
}

// --------- FUNCTIONS TO BE MOVED -----------
    
/** @brief Generate index.rst with table of contents for documentation files.
 *
 * Generates an `index.rst` file with a table of contents that includes all
 * of the generated module documentation files.
 * 
 * @param[in] location Output location for the index.rst file.
 * @param[in] toc Vector of file names for generated documentation files.
 */
int generate_index_file(fs::path location, std::vector<std::string> toc)
{
    fs::path file_path = location;
    file_path /= "index.rst";
    
    // Generate index.rst text
    std::string index = ".. toctree::\n   :maxdepth: 1\n";

    for (std::string file : toc)
    {
	index += std::string("\n   ") + file;
    }
    
    // Write to file at the specified location
    return write_file(file_path, index);
}

/** @brief Replace invalid characters in a string.
 *
 * Replaces the provided invalid characters with the provided replacement
 * characters.
 * 
 * @param[in] str String to replace characters in.
 * @param[in] invalid_chars List of invalid characters.
 * @param[in] replacement Replacement character for invalid charactes.
 *
 * @returns String with invalid characters replaced.
 */
std::string replace_invalid_chars(const std::string str,
				  const std::string invalid_chars,
				  const char replacement)
{
    std::string valid_str = str;

    // Loop over list of invalid chars and replace them
    for (auto invalid_char : invalid_chars)
    {
	std::replace(valid_str.begin(), valid_str.end(),
		     invalid_char, replacement); 
    }
    
    return valid_str;
}

/** @brief Write a file with given text to disk.
 *
 * Write a file with given text to disk.
 * 
 * @param[in] file_path Relative or absolute path to the file to be written.
 * @param[in] text Text to write to the file.
 *
 * @returns 0 - success; -1 - Error opening file
 */
int write_file(std::string file, std::string text)
{
    std::ofstream fout(file);

    // Check for error on opening file
    if (!fout)
    {
	return -1;
    }

    fout << text;

    fout.close();

    return 0;
}


} // namespace sde::printing
