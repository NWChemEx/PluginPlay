#include "sde/detail_/rest_printer.hpp"
#include <iterator>
#include <sstream>

namespace sde::detail_ {

reSTPrinter::reSTPrinter(std::ostream& os, std::string underline_chars,
                         std::string overline_chars) noexcept :
  m_underline_chars_(std::move(underline_chars)),
  m_overline_chars_(std::move(overline_chars)),
  m_os_(os) {}

void reSTPrinter::start_section(const std::string& header) {
    if(m_underline_chars_.size() <= m_section_)
        throw std::runtime_error("Not enough underline characters");
    if(m_overline_chars_.size() <= m_section_)
        throw std::runtime_error("Not enough overline characters");

    const auto ochar  = m_overline_chars_[m_section_];
    const auto uchar  = m_underline_chars_[m_section_];
    const auto nchars = header.size();
    if(ochar != ' ') m_os_.get() << std::string(nchars, ochar) << std::endl;
    m_os_.get() << header << std::endl;
    m_os_.get() << std::string(nchars, uchar) << std::endl;
    ++m_section_;
}

void reSTPrinter::finish_section() {
    if(m_section_ == 0) throw std::runtime_error("Not in a section");
    --m_section_;
}

reSTPrinter& reSTPrinter::operator<<(const std::string& message) {
    /* There's probably a better way to do this without copying things into the
     * vectors. I copy into the vectors because I need to know how many
     * sentences/words I have to properly handle termination.
     */
    using itr_type = std::istream_iterator<std::string>;
    std::istringstream iss(message);
    std::string s;
    std::vector<std::string> sentences;
    while(getline(iss, s)) { sentences.emplace_back(std::move(s)); }

    const auto nsent = sentences.size();
    for(std::size_t si = 0; si < nsent; ++si) { // Loop over sentences
        // Break lines into words
        std::istringstream iss2(sentences[si]);
        std::vector<std::string> words;
        words.insert(words.end(), itr_type(iss2), itr_type());

        unsigned short nchars = 0;
        for(const auto& wordi : words) { // Loop over words
            const unsigned short size = wordi.size();
            const bool fits           = nchars + size <= 80;

            // If it fits and there's characters we need a space
            if(nchars > 0 && fits) {
                m_os_.get() << ' ';
                ++nchars;
            } else if(!fits) { // If it doesn't fit put it on next line
                m_os_.get() << std::endl;
                nchars = 0;
            }

            // Print the word
            m_os_.get() << wordi;
            nchars += size;
        } // loop over words

        // Print a newline if we have more lines left
        if(si < nsent - 1) { m_os_.get() << std::endl; }
    } // loop over lines
    return *this;
}

} // namespace sde::detail_
