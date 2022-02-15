#include "pluginplay/any/detail_/any_result_wrapper.hpp"
#include <sstream>

#include "../test_any.hpp"

using namespace pluginplay::detail_;

/* Testing Strategy:
 *
 * All of the base classes in the any hierarchy are abstract except for the
 * most derived ones. Hence we need to create instances of the most derived
 * classes to test the base classes. In this file we only unit test functions
 * which are overriden in the AnyFieldWrapper and use other files to unit test
 * functions overriden elsewhere in the hierarchy. We test the AnyFieldWrapper
 */

TEMPLATE_LIST_TEST_CASE("AnyFieldWrapper", "", testing::types2test) {
    using type                = TestType;
    using result_wrapper_type = AnyResultWrapper<type>;
    using rtti_type           = typename result_wrapper_type::rtti_type;

    rtti_type rtti(typeid(type));

    auto value = testing::non_default_value<type>();

    result_wrapper_type defaulted(type{});
    result_wrapper_type has_value(value);

    SECTION("type") {
        REQUIRE(defaulted.type() == rtti);
        REQUIRE(has_value.type() == rtti);
    }

    SECTION("print") {
        std::stringstream ss;

        auto pss = &(has_value.print(ss));
        REQUIRE(pss == &ss);

        if constexpr(std::is_same_v<type, int>) {
            REQUIRE(ss.str() == "42");
        } else if constexpr(std::is_same_v<type, double>) {
            REQUIRE(ss.str() == "3.14");
        } else if constexpr(std::is_same_v<type, std::string>) {
            REQUIRE(ss.str() == "Hello World");
        }
    }
}
