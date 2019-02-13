#include "examples/TestModuleBase.hpp"
#include <SDE/SubmoduleRequest.hpp>
#include <catch2/catch.hpp>

using namespace SDE;
using namespace SDE::detail_;

TEST_CASE("SubmoduleRequest : default ctor") {
    SubmoduleRequest request;
    SECTION("State") {
        REQUIRE(request.description() == "");
        REQUIRE(!request.ready());
    }
}

TEST_CASE("SubmoduleRequest : comparisons") {
    SubmoduleRequest request1, request2;
    SECTION("Default state") {
        REQUIRE(request1 == request2);
        REQUIRE(!(request1 != request2));
    }
}

TEST_CASE("SubmoduleRequest : set_description") {
    SubmoduleRequest request1, request2;
    request1.set_description("hello world");
    REQUIRE(request1.description() == "hello world");
    REQUIRE(request1 != request2);
    REQUIRE(!(request1 == request2));
}
