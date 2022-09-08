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

#pragma once
#include <cstring>
#include <fort.hpp>

namespace pluginplay::printing::detail_ {

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

} // namespace pluginplay::printing::detail_
