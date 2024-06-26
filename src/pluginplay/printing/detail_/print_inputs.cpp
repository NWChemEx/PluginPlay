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
#include "print_inputs.hpp"
#include <fort.hpp>
namespace pluginplay::printing::detail_ {

reSTPrinter& input_desc(reSTPrinter& p) {
    p << "This section details the full list of inputs that the module accepts."
         " Inputs that are not listed as optional must have their value set "
         "before the module can be run. Inputs can be set manually by calling "
         "the module's ``change_input`` function or by passing arguments to a "
         "property type.";
    return p;
}

reSTPrinter& input_quick_ref_desc(reSTPrinter& p) {
    p << "The following table provides a quick summary of the inputs "
         "available to this module. The columns are:\n\n"
         "- ``Key``: The key used to retrieve the option,\n"
         "- ``Default`` : The initial value, if one exists, and\n"
         "- ``Description`` : A human readable description of what the option"
         " is used for.\n";
    return p;
}

std::string input_quick_ref_table(const type::input_map& inputs) {
    // Instantiate the table
    fort::utf8_table table;
    table.set_border_style(NWX_RST_STYLE);

    // Add the header
    table << fort::header << "Key"
          << "Default"
          << "Description" << fort::endr;

    // Add all data rows
    for(const auto& [name, value] : inputs) {
        table << name << (value.has_value() ? value.str() : "N/A")
              << (value.has_description() ? value.description() : "N/A")
              << fort::endr;
    }

    return table.to_string();
}

reSTPrinter& input_full_list_desc(reSTPrinter& p) {
    p << "The following subsections provide detailed descriptions of each input"
         " recognized by this module. The subsection headings are the name of "
         "the input. Within each subsection we list:\n\n"
         "- ``Description`` : A human readable description of what the option"
         " is used for.\n"
         "- ``Default Value`` : The initial value, if one exists.\n"
         "- ``Optional?`` : Whether the value must be set inorder for the "
         "module to run.\n"
         "- ``Opaque?`` : Does the value of the input influence memoization?.\n"
         "- ``Domain Restrictions`` : Criteria a value must obey to be deemed "
         "valid.";
    return p;
}

reSTPrinter& input_full_list(reSTPrinter& p, const type::input_map& inputs) {
    for(const auto& [name, input] : inputs) {
        p.start_section(name);
        p << "\n";
        p.print_verbatim(
          std::string("- Description : ") +
          (input.has_description() ? input.description() : "N/A") + "\n");
        p << std::string("- Default Value : ") +
               (input.has_value() ? input.str() : "N/A") + "\n";
        p << std::string("- Optional? : ") +
               (input.is_optional() ? "True" : "False") + "\n";
        p << std::string("- Opaque? : ") +
               (input.is_transparent() ? "False" : "True") + "\n";

        auto checks = input.check_descriptions();
        if(checks.empty())
            p << "- Domain Restrictions : N/A\n";
        else {
            p << "- Domain Restrictions :\n\n";
            for(auto& desc : checks) {
                auto p1 = std::string("  - ") + desc + "\n\n";
                p.print_verbatim(p1);
            }
        }
        p.finish_section();
    }
    return p;
}

reSTPrinter& print_inputs(reSTPrinter& p, const type::input_map& inputs) {
    p.start_section("Module Inputs");
    p << "\n";

    input_desc(p);

    p << "\n\n";

    p.start_section("Quick Reference");

    p << "\n";

    if(inputs.empty())
        p << "The module defines no inputs.\n\n";
    else {
        input_quick_ref_desc(p);
        p << "\n";
        p.print_verbatim(input_quick_ref_table(inputs));
        p << "\n\n";
    }
    p.finish_section(); // end quick reference

    p.start_section("Detailed Descriptions");

    if(inputs.empty())
        p << "\nThe module defines no inputs.\n";
    else {
        p << "\n";
        input_full_list_desc(p);
        p << "\n\n";
        input_full_list(p, inputs);
    }

    p.finish_section(); // end detailed descriptions
    p.finish_section(); // end module inputs

    return p;
}

} // namespace pluginplay::printing::detail_
