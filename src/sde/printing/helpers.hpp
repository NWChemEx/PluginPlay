#pragma once
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <vector>

namespace sde::printing {

/** @brief Generate index.rst with table of contents for documentation files.
 *
 * Generates text for an `index.rst` file with a table of contents that includes
 * all of the generated module documentation files.
 * 
 * @param[in] os std::ostream to write the text to.
 * @param[in] toc Vector of file names for documentation files without ".rst".
 */
void generate_index_file(std::ostream& os,
			 const std::vector<std::string>& toc);

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
std::string replace_invalid_chars(const std::string& str,
				  const std::string& invalid_chars,
				  const char& replacement);
    
} // namespace sde::printing
