#include "sde/printing/detail_/rest_printer.hpp"
#include <iterator>
#include <sstream>

namespace sde::printing::detail_ {

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
    m_os_.get() << message;
    return *this;
}

} // namespace sde::printing::detail_
