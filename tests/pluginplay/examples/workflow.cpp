#include "workflow.hpp"
#include <catch2/catch.hpp>
#include <vector>

TEST_CASE("load_modules") {
    pluginplay::ModuleManager mm;
    load_modules(mm);
    REQUIRE(mm.count("Rectangle") == 1);
    REQUIRE(mm.count("Prism") == 1);
}

TEST_CASE("example_main") { REQUIRE_NOTHROW(example_main()); }
