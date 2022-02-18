#include "pluginplay/any/detail_/any_input_wrapper.hpp"
#include <map>

#include "../test_any.hpp"

using namespace pluginplay::detail_;

TEST_CASE("is_any_input_wrapper") {
    using vector_t = std::vector<int>;
    STATIC_REQUIRE_FALSE(is_any_input_wrapper<int>::value);
    STATIC_REQUIRE_FALSE(is_any_input_wrapper<vector_t>::value);
    STATIC_REQUIRE(is_any_input_wrapper<AnyInputWrapper<int>>::value);
    STATIC_REQUIRE(is_any_input_wrapper<AnyInputWrapper<vector_t>>::value);
}

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
    using map_type           = std::map<int, int>;

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

        REQUIRE(defaulted.template cast<const type&>() == type{});
        REQUIRE(has_value.template cast<const type&>() == value);
        REQUIRE(const_val.template cast<const type&>() == value);
        REQUIRE(&const_ref.template cast<const type&>() == &value);

        // Ensure we can move complicated types when stored by value
        if constexpr(std::is_same_v<type, std::vector<int>>) {
            const auto corr = value.data();
            input_wrapper_type moved(std::move(value));
            auto p = moved.template cast<const type&>().data();
            REQURIE(corr == p);
        }
    }

    SECTION("Copy CTor") {
        input_wrapper_type defaulted2(defaulted);
        REQUIRE(defaulted2.are_equal(defaulted));

        input_wrapper_type has_value2(has_value);
        REQUIRE(has_value2.are_equal(has_value));

        cval_input_wrapper const_val2(const_val);
        REQUIRE(const_val2.are_equal(const_val));

        cref_input_wrapper const_ref2(const_ref);
        REQUIRE(const_ref2.are_equal(const_ref));

        // Still aliases the same instance
        auto corr = &const_ref.template cast<const type&>();
        REQUIRE(corr == &value);
        REQUIRE(&const_ref2.template cast<const type&>() == corr);
    }

    SECTION("clone") {
        auto defaulted2 = defaulted.clone();
        REQUIRE(defaulted2->are_equal(defaulted));

        auto has_value2 = has_value.clone();
        REQUIRE(has_value2->are_equal(has_value));

        auto const_val2 = const_val.clone();
        REQUIRE(const_val2->are_equal(const_val));

        auto const_ref2 = const_ref.clone();
        REQUIRE(const_ref2->are_equal(const_ref));

        // Still aliases the same instance
        auto corr = &const_ref.template cast<const type&>();
        REQUIRE(corr == &value);
        REQUIRE(&const_ref2->template cast<const type&>() == corr);
    }

    SECTION("are_equal") {
        // Defaults are equal
        REQUIRE(defaulted.are_equal(input_wrapper_type{type{}}));

        // Values are equal
        REQUIRE(has_value.are_equal(input_wrapper_type{value}));

        // Default != value
        REQUIRE_FALSE(defaulted.are_equal(has_value));

        // Value != const Value
        REQUIRE_FALSE(has_value.are_equal(const_val));

        // Value != const ref
        REQUIRE_FALSE(has_value.are_equal(const_ref));

        // const value != const ref
        REQUIRE_FALSE(const_val.are_equal(const_ref));
    }

    SECTION("do_hash") {
        // Since function is called do_hash can't use hash_objects
        using pluginplay::hash_to_string;
        auto h0 = pluginplay::make_hasher();
        auto h1 = pluginplay::make_hasher();
        auto h2 = pluginplay::make_hasher();
        auto h3 = pluginplay::make_hasher();
        defaulted.do_hash(h0);
        has_value.do_hash(h1);
        const_val.do_hash(h2);
        const_ref.do_hash(h3);

        auto h_default = hash_to_string(h0.finalize());
        auto h_value   = hash_to_string(h1.finalize());
        auto h_cval    = hash_to_string(h2.finalize());
        auto h_cref    = hash_to_string(h3.finalize());

        // Defaults are equal
        auto h4 = pluginplay::make_hasher();
        input_wrapper_type(type{}).do_hash(h4);
        auto h_default2 = hash_to_string(h4.finalize());
        REQUIRE(h_default == h_default2);

        // Values are equal
        auto h5 = pluginplay::make_hasher();
        input_wrapper_type(value).do_hash(h5);
        auto h_value2 = hash_to_string(h5.finalize());
        REQUIRE(h_value == h_value2);

        // Default != value
        REQUIRE(h_default != h_value);

        // Value == const Value
        REQUIRE(h_value == h_cval);

        // Value == const ref
        REQUIRE(h_value == h_cref);

        // const value == const ref
        REQUIRE(h_cval == h_cref);
    }

    SECTION("storing_reference") {
        REQUIRE_FALSE(defaulted.storing_const_ref());
        REQUIRE_FALSE(has_value.storing_const_ref());
        REQUIRE_FALSE(const_val.storing_const_ref());
        REQUIRE(const_ref.storing_const_ref());
    }

    SECTION("storing_const_value") {
        REQUIRE_FALSE(defaulted.storing_const_value());
        REQUIRE_FALSE(has_value.storing_const_value());
        REQUIRE(const_val.storing_const_value());
        REQUIRE_FALSE(const_ref.storing_const_value());
    }
}
