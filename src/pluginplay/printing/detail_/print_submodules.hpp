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
#include "pluginplay/submodule_request.hpp"
#include "pluginplay/types.hpp"
#include "rest_printer.hpp"

namespace pluginplay::printing::detail_ {

/** @brief Adds boiler plate description for the submodule section printer
 *
 *  This function contains the boiler plate introduction for introducing the
 *  the section describing the submodules that a module uses.
 *
 * @param[in,out] p The reSTPrinter to add the prose to. Assumed to be in the
 *                  correct section already.
 *
 * @return @p with its internal state modified to contain the intro to the
 *            input section.
 *
 * @throw ??? If the call to ostream::operator<< wrapped in @p throws given the
 *            description. Same guarantee.
 */
reSTPrinter& submod_desc(reSTPrinter& p);

/** @brief Makes a table of the submodules a module uses
 *
 *  This function will loop over the submodules and produce a table suitable for
 *  display in documentation. This table will include the key the submodule is
 *  assigned to, the property type it must satisfy, and a description of what
 *  the submodule is used for.
 *
 * @param[in] submods The submodule_map of submodule requests to table-ize.
 *
 * @return A string containing the formatted table.
 *
 * @throw std::bad_alloc if there is insufficient memory to parse the
 *                       submodules. Strong throw guarantee.
 * @throw ??? If utilities::Table throws during the process of turning the
 *            data into a table. Same throw guarantee.
 */
std::string submod_table(const type::submodule_map& submods);

/** @brief Driver for printing the submodules section of the documentation
 *
 *  This function will assemble and add to the provided printer the section
 *  detailing a module's submodules.
 *
 * @param[in,out] p The printer to add the prose to.
 * @param[in] submods The submodule_map containing the submodule requests that
 *                    the module specifies.
 *
 * @return @p with its internal state modified to store the submodules section.
 *
 * @throw std::bad_alloc if there is insufficient memory to parse the
 *                       submodules. Weak throw guarantee.
 *
 * @throw ??? if adding any of the strings to the printer or the table throws.
 *            Weak throw guarantee.
 */
reSTPrinter& print_submods(reSTPrinter& p, const type::submodule_map& submods);

} // namespace pluginplay::printing::detail_
