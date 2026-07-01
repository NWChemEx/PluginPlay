/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
