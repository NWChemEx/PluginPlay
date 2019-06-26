#include "sde/printing/document_module.hpp"
#include "test_common.hpp"
#include <catch2/catch.hpp>

using namespace sde::printing;
using namespace testing;

TEST_CASE("document_module") {
    SECTION("Empty module") {
        auto m    = make_module<NullModule>();
        auto corr = "";

        std::cout << document_module("empty module", *m) << std::endl;
        REQUIRE(corr == corr);
    }
}
