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
#include <pluginplay/fields/module_input.hpp>
#include <pluginplay/types.hpp>

namespace pluginplay::printing::detail_ {

/** @brief Adds boiler plate description for the module inputs to the printer
 *
 *  This function contains the boiler plate introduction for introducing the
 *  entirety of the input section.
 *
 * @param[in,out] p The reSTPrinter to add the prose to. Assumed to be in the
 *                  correct section already.
 * @return @p with its internal state modified to contain the intro to the
 *            input section.
 *
 * @throw ??? If the call to ostream::operator<< wrapped in @p throws given the
 *            description. Same guarantee.
 */
reSTPrinter& input_desc(reSTPrinter& p);

/** @brief Adds boiler plate quick reference description to printer
 *
 *  This function contains the boiler plate introduction for introducing the
 *  module input quick reference table.
 *
 * @param[in,out] p The reSTPrinter to add the prose to. Assumed to be in the
 *                  correct section already.
 * @return @p with its internal state modified to contain the intro to the
 *            quick reference section.
 *
 * @throw ??? If the call to ostream::operator<< wrapped in @p throws given the
 *            description. Same guarantee.
 */
reSTPrinter& input_quick_ref_desc(reSTPrinter& p);

/** @brief Makes a quick-reference table for the inputs
 *
 *  This function will loop over the inputs and produce a table suitable for
 *  quick reference. This table will include the name of the option, its default
 *  value (if it has one), and a description of what the option is used for.
 *
 * @param[i] inputs The input_map of module input fields to turn into a table.
 *
 * @return A string containing the formatted table.
 *
 * @throw std::bad_alloc if there is insufficient memory to parse the inputs.
 *                       Strong throw guarantee.
 * @throw ??? If utilities::Table throws during the process of turning the
 *            data into a table. Same throw guarantee.
 */
std::string input_quick_ref_table(const type::input_map& inputs);

/** @brief Adds boiler plate description for detailed input description to
 *         printer
 *
 *  This function contains the boiler plate introduction for introducing the
 *  detailed descriptions of the module's inputs.
 *
 * @param[in,out] p The reSTPrinter to add the prose to. Assumed to be in the
 *                  correct section already.
 * @return @p with its internal state modified to contain the intro to the
 *            detailed description section.
 *
 * @throw ??? If the call to ostream::operator<< wrapped in @p throws given the
 *            description. Same guarantee.
 */
reSTPrinter& input_full_list_desc(reSTPrinter& p);

/** @brief Appends detailed descriptions of each input to provided printer.
 *
 *  This function will loop over the inputs and produce subsections for each
 *  input field. Within each subsection the properties of that input field are
 *  listed.
 *
 * @param[in,out] p The reSTPrinter printer instance to write to.
 * @param[in] inputs The input_map of module input fields to process.
 *
 * @return @p with its internal state modified to include documentation for the
 *         inputs included in @p inputs.
 *
 * @throw ??? if the reSTPrinter instance's operator<< throws or if its
 *            `start_section` function throws. Weak throw guarantee.
 */
reSTPrinter& input_full_list(reSTPrinter& p, const type::input_map& inputs);

reSTPrinter& print_inputs(reSTPrinter& p, const type::input_map& inputs);

} // namespace pluginplay::printing::detail_
