#include "../catch.hpp"
#include "../test_common.hpp"
#include "../../../../../include/pluginplay/printing/mermaid.hpp"

TEST_CASE("hello_world") {
    SECTION("huh") {
        auto hello = hello_world();
        REQUIRE(hello == "Hello World!");
    }
}
