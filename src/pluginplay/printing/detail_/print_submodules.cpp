#include "fort_custom_styles.hpp"
#include "print_submodules.hpp"
#include <fort.hpp>
#include <utilities/printing/demangler.hpp>
#include <utilities/printing/table.hpp>

namespace pluginplay::printing::detail_ {

using Table = utilities::printing::Table;

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
    fort::char_table table;
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
