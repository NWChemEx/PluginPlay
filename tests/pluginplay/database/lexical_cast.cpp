#include "lexical_cast.hpp"
#include <catch2/catch.hpp>

using namespace testing;

TEST_CASE("lexical_cast") {
    REQUIRE(lexical_cast<int>("42") == int{42});
    REQUIRE(lexical_cast<double>("42") == double{42});
    REQUIRE(lexical_cast<std::string>("42") == std::string{"42"});
}
