#include "pluginplay/any/detail_/any_input_wrapper.hpp"

#include "../test_any.hpp"

using namespace pluginplay::detail_;

/* Testing Strategy:
 *
 * All of the base classes in the any hierarchy are abstract except for the
 * most derived ones. Hence we need to create instances of the most derived
 * classes to test the base classes. In this file we only unit test functions
 * which are overriden in the AnyInputWrapper and use other files to unit test
 * functions overriden elsewhere in the hierarchy.
 */

TEMPLATE_LIST_TEST_CASE("AnyInputWrapper", "", testing::types2test) {
    using type               = TestType;
    using input_wrapper_type = AnyInputWrapper<type>;
    using cval_input_wrapper = AnyInputWrapper<const type>;
    using cref_input_wrapper = AnyInputWrapper<const type&>;
    using rtti_type          = typename input_wrapper_type::rtti_type;

    auto value = testing::non_default_value<type>();

    input_wrapper_type defaulted(type{});
    input_wrapper_type has_value(value);
    cval_input_wrapper const_val(value);
    cref_input_wrapper const_ref(value);

    SECTION("Value CTor") {
        REQUIRE(defaulted.type() == rtti_type(typeid(type)));
        REQUIRE(has_value.type() == rtti_type(typeid(type)));
        REQUIRE(const_val.type() == rtti_type(typeid(const type)));
        REQUIRE(const_ref.type() == rtti_type(typeid(const type&)));
    }
}
