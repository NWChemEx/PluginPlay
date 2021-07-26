#pragma once
#include <cstring>
#include <fort.hpp>

namespace sde::printing::detail_ {

/** @brief Restructured text table border style for libfort tables.
 *
 * Libfort does not have a default table border style to match restructured text
 * grid tables, so a custom border style is defined.
 *
 * How to use this style to set the border style of a table:
 *   - C API   : ft_set_border_style(table, NWX_RST_STYLE);
 *   - C++ API : table.set_border_style(NWX_RST_STYLE);
 */
extern const struct ft_border_style* const NWX_RST_STYLE;

} // namespace sde::printing::detail_
