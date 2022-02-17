#include "pluginplay/any/any.hpp"

#include "test_any.hpp"

using namespace pluginplay;

/* Testing Strategy:
 *
 * AnyResult adds result-specific functions to the AnyField class. These unit
 * tests only focus on testing the functions implemented in the AnyResult class.
 * Functions implemented in the AnyField class are unit tested in any_field.cpp.
 * We use make_any_result to create AnyResult instances and any_cast to unwrap
 * them, the tests here assume they have already been tested (they are tested in
 * any.cpp).
 */

TEMPLATE_LIST_TEST_CASE("AnyResult", "", testing::types2test) {
    using type        = TestType;
    using result_type = AnyResult;
    using rtti_type   = typename result_type::rtti_type;

    auto value = testing::non_default_value<type>();

    result_type defaulted;
    auto has_value = make_any_result<type>(value);

    SECTION("CTors") {
        // Check the state of a default constructed instance
        SECTION("Default Ctor") {
            REQUIRE_FALSE(defaulted.has_value());
            REQUIRE(defaulted.type() == rtti_type(typeid(nullptr)));
            REQUIRE_THROWS_AS(any_cast<type>(defaulted), std::runtime_error);
        }

        // Check the state of a value constructed instance
        SECTION("Value Ctor") {
            REQUIRE(has_value.has_value());
            REQUIRE(has_value.type() == rtti_type(typeid(type)));
            REQUIRE(any_cast<type>(has_value) == value);
        }
    }

    SECTION("has_value") {
        // Ensure defaulted instance registers as not having a value
        REQUIRE_FALSE(defaulted.has_value());

        // Ensure value-wrapping instance registers as havine a value
        REQUIRE(has_value.has_value());
    }
}
