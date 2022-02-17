#include "pluginplay/any/detail_/any_result_wrapper.hpp"

#include "../test_any.hpp"

using namespace pluginplay::detail_;

/* Testing Strategy:
 *
 * All of the base classes in the any hierarchy are abstract except for the
 * most derived ones. Hence we need to create instances of the most derived
 * classes to test the base classes. In this file we only unit test functions
 * which are implemented in AnyFieldBase and use other files to unit test
 * functions overriden elsewhere in the hierarchy.
 */

TEMPLATE_LIST_TEST_CASE("AnyResultBase", "", testing::types2test) {
    using type                = TestType;
    using result_wrapper_type = AnyResultWrapper<type>;

    auto value = testing::non_default_value<type>();

    result_wrapper_type defaulted(type{});
    result_wrapper_type has_value(value);

    SECTION("cast") {
        // Uncomment to test static asserts
        // auto x = defaulted.template cast<type&>();

        REQUIRE(defaulted.template cast<const type>() == type{});
        REQUIRE(defaulted.template cast<type>() == type{});
        REQUIRE(defaulted.template cast<const type&>() == type{});
        REQUIRE(has_value.template cast<const type>() == value);
        REQUIRE(has_value.template cast<type>() == value);
        REQUIRE(has_value.template cast<const type&>() == value);
    }

    SECTION("is_convertible") {
        REQUIRE(defaulted.template is_convertible<const type>());
        REQUIRE(defaulted.template is_convertible<type>());
        REQUIRE(defaulted.template is_convertible<const type&>());
        REQUIRE_FALSE(defaulted.template is_convertible<type&>());
        REQUIRE_FALSE(defaulted.template is_convertible<type*>());
    }
}
