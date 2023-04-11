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
#include "rest_printer.hpp"
#include <pluginplay/fields/module_result.hpp>
#include <pluginplay/types.hpp>

namespace pluginplay::printing::detail_ {

/** @brief Adds boiler plate description for the module results to the printer
 *
 *  This function contains the boiler plate introduction for introducing the
 *  entirety of the module results section.
 *
 * @param[in,out] p The reSTPrinter to add the prose to. Assumed to be in the
 *                  correct section already.
 * @return @p with its internal state modified to contain the intro to the
 *            results section.
 *
 * @throw ??? If the call to ostream::operator<< wrapped in @p throws given the
 *            description. Same guarantee.
 */
reSTPrinter& result_desc(reSTPrinter& p);

/** @brief Makes a table for the results
 *
 *  This function will loop over the results and produce a table suitable for
 *  quick reference. This table will include the name of the result, its type,
 *  and a description of what the result is/how it was computed.
 *
 * @param[in] results The result_map to turn into a table.
 *
 * @return A string containing the formatted table.
 *
 * @throw std::bad_alloc if there is insufficient memory to parse the results.
 *                       Strong throw guarantee.
 * @throw ??? If utilities::Table throws during the process of turning the
 *            data into a table. Same throw guarantee.
 */
std::string result_table(const type::result_map& results);

/** @brief Convenience function for printing out the entire results section
 *
 *  This function will print the banner starting the module results section, a
 *  short description of the table used to organize the results, and then the
 *  results.
 *
 *  @param[in,out] p The printer to add this section to.
 *  @param[in] results The result_map containing results that we should print.
 *
 * @return @p p with its internal state updated to include the results section.
 */
reSTPrinter& print_results(reSTPrinter& p, const type::result_map& results);

} // namespace pluginplay::printing::detail_
