#include "tests/examples/workflow.hpp"
#include <catch2/catch.hpp>
#include <vector>

using namespace sde;

TEST_CASE("load_modules") {
    sde::ModuleManager mm;
    sde::load_modules(mm);
    REQUIRE(mm.count("Rectangle") == 1);
    REQUIRE(mm.count("Prism") == 1);
}

TEST_CASE("example_main") { REQUIRE_NOTHROW(example_main()); }
