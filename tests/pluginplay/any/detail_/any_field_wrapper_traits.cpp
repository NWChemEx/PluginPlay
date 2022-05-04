#include "../test_any.hpp"
#include "pluginplay/any/detail_/any_field_wrapper_traits.hpp"

using namespace pluginplay::any::detail_;

TEMPLATE_TEST_CASE("is_any_field_wrapper", "", testing::types2test) {
    using T            = TestType;
    using wrapper_type = AnyFieldWrapper<T>;
    STATIC_REQUIRE_FALSE(is_any_field_wrapper<T>::value);
    STATIC_REQUIRE(is_any_field_wrapper<wrapper_type>::value);
}

TEMPLATE_TEST_CASE("disable_if_any_field_wrapper_t", "", testing::types2test) {
    using T            = TestType;
    using wrapper_type = AnyFieldWrapper<T>;
    STATIC_REQUIRE(std::is_same_v<int, disable_if_any_field_wrapper_t<T, int>>);
    // Should fail to compile, uncomment to check
    // STATIC_REQUIRE(
    //  std::is_same_v<int, disable_if_any_field_wrapper_t<wrapper_type>>);
}
