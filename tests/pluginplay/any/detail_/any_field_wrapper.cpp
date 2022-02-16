#include "pluginplay/any/detail_/any_input_wrapper.hpp"
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
    using input_wrapper_type  = AnyInputWrapper<type>;
    using cval_input_wrapper  = AnyInputWrapper<const type>;
    using cref_input_wrapper  = AnyInputWrapper<const type&>;
    using rtti_type           = typename result_wrapper_type::rtti_type;

    rtti_type rtti(typeid(type));

    auto value = testing::non_default_value<type>();

    result_wrapper_type rdefaulted(type{});
    result_wrapper_type rhas_value(value);

    input_wrapper_type idefaulted(type{});
    input_wrapper_type ihas_value(value);
    cval_input_wrapper const_val(value);
    cref_input_wrapper const_ref(value);

    SECTION("type") {
        REQUIRE(rdefaulted.type() == rtti);
        REQUIRE(rhas_value.type() == rtti);
        REQUIRE(idefaulted.type() == rtti_type(typeid(type)));
        REQUIRE(ihas_value.type() == rtti_type(typeid(type)));
        REQUIRE(const_val.type() == rtti_type(typeid(const type)));
        REQUIRE(const_ref.type() == rtti_type(typeid(const type&)));
    }

    SECTION("print") {
        std::stringstream ss;

        SECTION("input") {
            auto pss = &(ihas_value.print(ss));
            REQUIRE(pss == &ss);

            if constexpr(std::is_same_v<type, int>) {
                REQUIRE(ss.str() == "42");
            } else if constexpr(std::is_same_v<type, double>) {
                REQUIRE(ss.str() == "3.14");
            } else if constexpr(std::is_same_v<type, std::string>) {
                REQUIRE(ss.str() == "Hello World");
            }
        }

        SECTION("result") {
            auto pss = &(rhas_value.print(ss));
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
}
