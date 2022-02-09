#include "pluginplay/any/detail_/any_result_wrapper.hpp"
#include <catch2/catch.hpp>
#include <map>

using namespace pluginplay::detail_;

/* Testing Strategy:
 *
 * All of the base classes in the any hierarchy are abstract except for the
 * most derived ones. Hence we need to create instances of the most derived
 * classes to test the base classes. In this file we only unit test functions
 * which are overriden in the AnyResultWrapper and use other files to unit test
 * functions overriden elsewhere in the hierarchy.
 */

using types2test = std::tuple<int, double, std::string, std::vector<double>>;

template<typename T>
auto default_value() {
    if constexpr(std::is_same_v<int, T>) {
        return int{42};
    } else if constexpr(std::is_same_v<double, T>) {
        return double{3.14};
    } else if constexpr(std::is_same_v<std::string, T>) {
        return std::string{"Hello World"};
    } else if constexpr(std::is_same_v<std::vector<double>, T>) {
        return std::vector<double>{1.2, 2.3, 3.4};
    } else {
        static_assert(std::is_same_v<T, int>, "Register your type!!!!");
    }
}

TEMPLATE_LIST_TEST_CASE("AnyResultWrapper", "", types2test) {
    using type                = TestType;
    using result_wrapper_type = AnyResultWrapper<type>;
    using rtti_type           = typename result_wrapper_type::rtti_type;

    rtti_type rtti(typeid(type));
    auto value = default_value<type>();

    // ---------- uncomment to verify state_asserts on types -------------------
    // AnyResultWrapper<const type> is_const(value);
    // AnyResultWrapper<type&> is_ref(value);
    // AnyResultWrapper<const type&> is_const_ref(value);

    result_wrapper_type defaulted(type{});
    result_wrapper_type has_value(value);

    SECTION("Value CTor") {
        REQUIRE(defaulted.has_value());
        REQUIRE(has_value.has_value());
        REQUIRE(defaulted.type() == rtti);
        REQUIRE(has_value.type() == rtti);
    }

    SECTION("Clone") {}

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
