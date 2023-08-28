#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <iostream>

int print_and_return(int ii) {
    std::cout << "This is only a blank model for an integration test."
              << std::endl;
    return 0;
}

TEST_CASE("Blank test", "[classic]") { REQUIRE(print_and_return(0) == 0); }
