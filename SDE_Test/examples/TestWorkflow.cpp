#include "examples/TestWorkflow.hpp"
#include <catch2/catch.hpp>

TEST_CASE("load_modules") {
    SDE::ModuleManager mm;
    load_modules(mm);
    REQUIRE(mm.count("Rectangle") == 1);
    REQUIRE(mm.count("Prism") == 1);
}

TEST_CASE("example_main") { REQUIRE_NOTHROW(example_main()); }
