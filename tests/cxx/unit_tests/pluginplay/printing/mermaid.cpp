#include <iostream>
#include "../catch.hpp"
#include "../test_common.hpp"
#include "pluginplay/printing/mermaid.hpp"

using namespace testing;

TEST_CASE("hello_world") {
    SECTION("huh") {
        auto hello = "Hello World!";
        REQUIRE(hello_world() == "Hello World!");
    }
}
