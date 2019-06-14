#include <catch2/catch.hpp>
#include <sde/detail_/printing.hpp>

using namespace sde::detail_;

TEST_CASE("print_options") {
    sde::type::input_map inputs;
    inputs["Option 1"];
    std::cout << print_options(inputs);
}
