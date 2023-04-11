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
#include "print_results.hpp"
#include <fort.hpp>
#include <utilities/printing/demangler.hpp>
namespace pluginplay::printing::detail_ {

reSTPrinter& result_desc(reSTPrinter& p) {
    p << "This section tabulates the full list of results that the module "
         "returns. The columns respectively are:\n\n"
         "- Key: What the result is called\n"
         "- Type: The C++ type of the result\n"
         "- Description: What the result is/how it was computed.\n\n"
         ".. note::\n\n"
         "   A given property type will only return a subset of the available "
         "results. \n"
         "   Additional results can be accessed by using other property types "
         "or by using\n"
         "   the Module class's advanced API.";
    return p;
}

std::string result_table(const type::result_map& results) {
    // Instantiate the table
    fort::char_table table;
    table.set_border_style(NWX_RST_STYLE);

    // Add the header
    table << fort::header << "Key"
          << "Type"
          << "Description" << fort::endr;

    // Add all data rows
    for(const auto& [name, value] : results) {
        table << name
              << (value.has_type() ?
                    utilities::printing::Demangler::demangle(value.type()) :
                    "N/A")
              << (value.has_description() ? value.description() : "N/A")
              << fort::endr;
    }

    return table.to_string();
}

reSTPrinter& print_results(reSTPrinter& p, const type::result_map& inputs) {
    p.start_section("Module Results");
    p << "\n";

    if(inputs.empty()) {
        p << "The module defines no results.\n\n";
        p.finish_section();
        return p;
    }

    result_desc(p);

    p << "\n\n";

    p.print_verbatim(result_table(inputs));

    p << "\n\n";

    p.finish_section(); // end module inputs

    return p;
}

} // namespace pluginplay::printing::detail_
