#include "helpers.hpp"

namespace sde::printing {

void generate_index_file(std::ostream& os,
                         const std::vector<std::string>& toc) {
    // Generate index.rst text
    std::string index = ".. toctree::\n   :maxdepth: 1\n";

    // Add file names to the toctree
    for(std::string file : toc) { index += std::string("\n   ") + file; }

    // Write to the std::ostream
    os << index;
}

std::string replace_invalid_chars(const std::string& str,
                                  const std::string& invalid_chars,
                                  const char& replacement) {
    std::string valid_str = str;

    // Loop over list of invalid chars and replace them
    for(auto invalid_char : invalid_chars) {
        std::replace(valid_str.begin(), valid_str.end(), invalid_char,
                     replacement);
    }

    return valid_str;
}

} // namespace sde::printing
