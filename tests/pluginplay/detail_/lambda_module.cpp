#include "../test_common.hpp"
#include "pluginplay/lambda_module.hpp"
#include <catch2/catch.hpp>

/* Testing strategy.
 *
 * For the most part the only moving part of the LambdaModule class is the
 * wrapping/unwrapping of the inputs/results to the user-provided function. The
 * unwrapping process defers to the PropertyType class and is thus tested there.
 * For the wrapping process we have to do a bit of template meta-programming
 * depending on whether we are wrapping one or multiple returns. This is what we
 * test in this source file
 */

TEST_CASE("LambdaModule") {
    SECTION("single return") {
        auto l = pluginplay::make_lambda<testing::OneOut>([]() { return 2; });
        REQUIRE(std::get<0>(l.run_as<testing::OneOut>()) == 2);
    }

    SECTION("multiple returns") {
        auto l = pluginplay::make_lambda<testing::TwoOut>(
          []() { return std::make_tuple(2, 'b'); });
        auto [i, c] = l.run_as<testing::TwoOut>();
        REQUIRE(i == 2);
        REQUIRE(c == 'b');
    }
}
