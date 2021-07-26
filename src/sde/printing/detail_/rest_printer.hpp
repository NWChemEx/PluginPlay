#pragma once
#include <functional>
#include <iostream>
#include <utilities/printing/word_wrap_stream.hpp>

namespace sde::printing::detail_ {

/** @brief Helps print out reST documentation
 *
 *  For the most part this class is a glorified std::ostream. The main
 *  difference is that it keeps track of what section number we are on. This
 *  allows us to pass the printer object recursively through our modules and
 *  have the sections, subsections, etc. come out correctly.
 */
class reSTPrinter {
public:
    /** @brief Wraps an existing std::ostream instance
     *
     *  @param[in] os the std::ostream instance to wrap. The data to print will
     *                be appended to that instance.
     * @param[in] underline_chars A string such that the i-th character is the
     *                            character to use for underlining a section at
     *                            depth i. Defaults to the Python reST order:
     *                            (`#`, `*`, `=`, `-`, `^`, `"`).
     * @param[in] overline_chars A string such that the i-th character is the
     *                           character to use for overlining a section at
     *                           depth i. A space is used to denote no overline.
     *                           Defaults to the Python reST order:
     *                           (`#`, `*`, ` `, ` `, ` `, ` `).
     *
     * @throw none No throw guarantee.
     */
    explicit reSTPrinter(std::ostream& os,
                         std::string underline_chars = "#*=-^\"",
                         std::string overline_chars  = "#*      ") noexcept;

    /** @brief Tells the printer to print a header for a new section.
     *
     *  This function will print a header for a new section. Here "section" is
     *  used generically and can be a chapter, section, subsection, or whatever.
     *  The resulting header will have an overline and underline determined by
     *  the current section nesting. More specifically if this is the i-th
     *  nested call the resulting section will have an overline/underline that
     *  is comprised of the i-th overline/underline character. When you are
     *  done writing this section make sure you call `finish_section`.
     *
     *
     *  @param[in] header The title of the current section. If the title exceeds
     *                    the page width no wrapping will occur.
     *
     *  @throw std::runtime_error if all underline or overline characters are
     *                            exhausted, i.e., the maximum section nesting
     *                            has been reached. Strong throw guarantee.
     *  @throw ??? if the underlying std::ostream throws. Weak throw guarantee.
     */
    void start_section(const std::string& header);

    /** @brief Tells the printer that you are done with the current section.
     *
     *  This function tells the printer that you are done writing the current
     *  section. Here "section" is a catch-all for chapter, section, subsection,
     *  etc. If you use start_section/end_section like they are open/close
     *  brackets, then this function will work as expected.
     *
     *  @throw std::runtime_error if the printer is not currently in a section.
     *                            Strong throw guarantee.
     */
    void finish_section();

    /** @brief Adds the provided message to the output buffer.
     *
     *  This function will add the provided text to the underlying output
     *  buffer, verbatim, without wrapping lines exceeding 80 characters.
     *
     *  @param[in] message What we should add to the buffer.
     *
     *  @return The current instance after adding @p message to the buffer
     *
     *  @throw ??? if the underlying std::ostream throws. Weak throw guarantee.
     */
    void print_verbatim(const std::string& message);

    /** @brief Adds the provided message to the wrapped buffer.
     *
     *  This function will add the provided text to the underlying buffer,
     *  verbatim, with the exception that lines exceeding 80 characters will be
     *  broken up (if possible; if a line contains a string with 80+ characters
     *  that string will be printed on one line).
     *
     *  @param[in] message What we should add to the buffer.
     *
     *  @return The current instance after adding @p message to the buffer
     *
     *  @throw ??? if the underlying std::ostream throws. Weak throw guarantee.
     */
    reSTPrinter& operator<<(const std::string& message);

private:
    /// The list of characters to use for underlining
    const std::string m_underline_chars_;

    /// The list of characters to use for overlining
    const std::string m_overline_chars_;

    /// The section depth we are currently at
    unsigned short m_section_ = 0;

    /// The stream we are printing to
    std::reference_wrapper<std::ostream> m_os_;

    /// The WordWrapStream to print to
    utilities::printing::WordWrapStream m_wws_;
}; // class reSTPrinter

} // namespace sde::printing::detail_
