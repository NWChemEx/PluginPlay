#pragma once
#include <functional>
#include <iostream>

namespace sde::detail_ {

class reSTPrinter {
public:
    explicit reSTPrinter(std::ostream& os,
                         std::string underline_chars = "*=-^\"",
                         std::string overline_chars  = "*      ");

    void start_section(const std::string& header);
    void finish_section();
    reSTPrinter& operator<<(const std::string& message);

private:
    const std::string m_underline_chars_;
    const std::string m_overline_chars_;
    unsigned short m_section_ = 0;
    std::reference_wrapper<std::ostream> m_os_;
};

} // namespace sde::detail_
