#pragma once
#include "sde/module_input.hpp"
#include "sde/types.hpp"
#include <utilities/printing/table.hpp>

namespace sde::detail_ {

std::string print_options(const type::input_map& inputs);

//-------------------------Implementations--------------------------------------
std::string print_options(const type::input_map& inputs) {
    using table_type = utilities::printing::Table;
    using table_data = typename table_type::table_type;

    // Take care of the column headers
    table_data data(inputs.size() + 1);
    data[0].push_back("Name");
    data[0].push_back("Description");
    data[0].push_back("Optional?");
    data[0].push_back("Opaque?");

    std::size_t counter = 1;
    for(const auto & [name, value] : inputs) {
        data[counter].push_back(name);
        data[counter].push_back(value.description());
        data[counter].push_back(value.is_optional() ? "True" : "False");
        data[counter].push_back(value.is_transparent() ? "False" : "True");
    }

    table_type t(std::move(data));

    // Compact the boolean columns as much as possible
    t.set_width(2, 9);
    t.set_width(3, 7);

    return t.str();
}

} // namespace sde::detail_
