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
#include "print_submodules.hpp"
#include <fort.hpp>
#include <utilities/printing/demangler.hpp>

namespace pluginplay::printing::detail_ {

reSTPrinter& submod_desc(reSTPrinter& p) {
    p << "This section details the full list of submodules that the module "
         "uses. For each submodule we have listed:\n\n"
         "- Key : The key used to refer to this particular callback.\n"
         "- Property Type : The property type that the submodule must "
         "satisfy.\n"
         "- Description : How the module will use the submodule.\n\n";
    return p;
}

std::string submod_table(const type::submodule_map& submods) {
    // Instantiate the table
    fort::utf8_table table;
    table.set_border_style(NWX_RST_STYLE);

    // Add the header
    table << fort::header << "Key"
          << "Property Type"
          << "Description" << fort::endr;

    // Add all data rows
    for(const auto& [name, value] : submods) {
        table << name
              << (value.has_type() ?
                    utilities::printing::Demangler::demangle(value.type()) :
                    "N/A")
              << (value.has_description() ? value.description() : "N/A")
              << fort::endr;
    }

    return table.to_string();
}

reSTPrinter& print_submods(reSTPrinter& p, const type::submodule_map& submods) {
    p.start_section("Submodules");
    p << "\n";

    if(submods.empty()) {
        p << "The module defines no submodules.\n\n";
        p.finish_section();
        return p;
    }

    submod_desc(p);

    p.print_verbatim(submod_table(submods));

    p.finish_section(); // end submodules

    return p;
}

} // namespace pluginplay::printing::detail_
