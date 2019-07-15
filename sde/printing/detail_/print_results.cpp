#include "sde/printing/detail_/print_results.hpp"
#include <utilities/printing/demangler.hpp>
#include <utilities/printing/table.hpp>

namespace sde::printing::detail_ {

using Table = utilities::printing::Table;

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

std::string result_table(const type::result_map& inputs) {
    using table_data = typename Table::table_type;

    // Take care of the column headers
    table_data data(inputs.size() + 1);
    data[0].push_back("Key");
    data[0].push_back("Type");
    data[0].push_back("Description");

    std::size_t counter = 1;
    for(const auto & [name, value] : inputs) {
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

    p << result_table(inputs) << "\n\n";

    p.finish_section(); // end module inputs

    return p;
}

} // namespace sde::printing::detail_
