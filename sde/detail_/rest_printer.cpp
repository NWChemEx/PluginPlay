#include "sde/detail_/rest_printer.hpp"
#include <sstream>

namespace sde::detail_ {

reSTPrinter::reSTPrinter(std::ostream& os, std::string underline_chars,
                         std::string overline_chars) :
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
    unsigned short nchars = 0;
    std::istringstream iss(message);
    std::string s;
    while(getline(iss, s, ' ')) {
        if(nchars + s.size() > 80) {
            m_os_.get() << std::endl;
            nchars = 0;
        }
        m_os_.get() << s << ' ';
        nchars += s.size() + 1;
    }
    m_os_.get() << std::endl;
    return *this;
}

} // namespace sde::detail_
