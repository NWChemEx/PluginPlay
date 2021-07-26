#include "fort_custom_styles.hpp"

namespace pluginplay::printing::detail_ {

// Internal definition
static const struct ft_border_style RST_STYLE = {
  {
    "-", // border.top_border_ch
    "-", // border.separator_ch
    "-", // border.bottom_border_ch
    "|", // border.side_border_ch
    "+", // border.out_intersect_ch
    "+", // border.in_intersect_ch
  },
  {
    "-", // header_border.top_border_ch
    "=", // header_border.separator_ch
    "-", // header_border.bottom_border_ch
    "|", // header_border.side_border_ch
    "+", // header_border.out_intersect_ch
    "+", // header_border.in_intersect_ch
  },
  "-" // border_style.hor_separator_char
};

const struct ft_border_style* const NWX_RST_STYLE = &RST_STYLE;

} // namespace pluginplay::printing::detail_
