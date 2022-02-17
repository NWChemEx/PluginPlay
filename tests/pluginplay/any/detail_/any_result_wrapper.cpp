#include "pluginplay/any/detail_/any_result_wrapper.hpp"
#include <map>

#include "../test_any.hpp"

using namespace pluginplay::detail_;

/* Testing Strategy:
 *
 * All of the base classes in the any hierarchy are abstract except for the
 * most derived ones. Hence we need to create instances of the most derived
 * classes to test the base classes. In this file we only unit test functions
 * which are overriden in the AnyResultWrapper and use other files to unit test
 * functions overriden elsewhere in the hierarchy.
 */

TEMPLATE_LIST_TEST_CASE("AnyResultWrapper", "", testing::types2test) {
    using type                = TestType;
    using result_wrapper_type = AnyResultWrapper<type>;
    using rtti_type           = typename result_wrapper_type::rtti_type;

    rtti_type rtti(typeid(type));
    auto value = testing::non_default_value<type>();

    // ---------- uncomment to verify state_asserts on types -------------------
    // AnyResultWrapper<const type> is_const(value);
    // AnyResultWrapper<type&> is_ref(value);
    // AnyResultWrapper<const type&> is_const_ref(value);

    result_wrapper_type defaulted(type{});
    result_wrapper_type has_value(value);

    SECTION("Value CTor") {
        REQUIRE(defaulted.type() == rtti);
        REQUIRE(has_value.type() == rtti);

        // If it's the vector make sure we can wrap it without a copy
        if constexpr(std::is_same_v<type, std::vector<double>>) {
            const auto pvalue = value.data();
            result_wrapper_type moved(std::move(value));
            const auto pmoved = moved.template cast<const type&>().data();
            REQUIRE(pmoved == pvalue);
        }
    }

    SECTION("Copy CTor") {
        result_wrapper_type defaulted2(defaulted);
        REQUIRE(defaulted2.are_equal(defaulted));

        result_wrapper_type has_value2(has_value);
        REQUIRE(has_value2.are_equal(has_value));
    }

    SECTION("Clone") {
        auto pdefaulted2 = defaulted.clone();
        REQUIRE(pdefaulted2->type() == rtti);
        REQUIRE(defaulted.are_equal(*pdefaulted2));

        auto phas_value2 = has_value.clone();
        REQUIRE(phas_value2->type() == rtti);
        REQUIRE(has_value.are_equal(*phas_value2));
    }

    SECTION("are_equal") {
        // Wraps the same type, but with different values
        REQUIRE_FALSE(defaulted.are_equal(has_value));

        // Wraps same type, with same values
        REQUIRE(defaulted.are_equal(result_wrapper_type(type{})));
        REQUIRE(has_value.are_equal(result_wrapper_type(value)));

        // Wraps different type
        using other_type = std::map<double, int>;
        AnyResultWrapper<other_type> other(other_type{});
        REQUIRE_FALSE(defaulted.are_equal(other));
    }
}
