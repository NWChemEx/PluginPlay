#include "sde/printing/detail_/print_submodules.hpp"
#include <utilities/printing/demangler.hpp>
#include <utilities/printing/table.hpp>

namespace sde::printing::detail_ {

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
    using table_data = typename Table::table_type;

    // Take care of the column headers
    table_data data(submods.size() + 1);
    data[0].push_back("Key");
    data[0].push_back("Property Type");
    data[0].push_back("Description");

    std::size_t counter = 1;
    for(const auto & [name, value] : submods) {
        auto& row = data[counter];
        row.push_back(name);
        row.push_back(value.has_type() ?
                        utilities::printing::Demangler::demangle(value.type()) :
                        "N/A");
        row.push_back(value.has_description() ? value.description() : "N/A");
        ++counter;
    }

    Table t1(std::move(data));
    return t1.str();
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

    p << submod_table(submods);

    p.finish_section(); // end submodules

    return p;
}

} // namespace sde::printing::detail_
