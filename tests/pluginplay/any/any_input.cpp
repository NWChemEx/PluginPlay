#include "pluginplay/any/any.hpp"

#include "test_any.hpp"

using namespace pluginplay;

TEMPLATE_LIST_TEST_CASE("AnyInput", "", testing::types2test) {
    using type       = TestType;
    using input_type = AnyInput;
    using rtti_type  = typename input_type::rtti_type;

    auto value = testing::non_default_value<type>();

    input_type defaulted;
    input_type by_value = make_any_input<type>(value);
    input_type by_cval  = make_any_input<const type>(value);
    input_type by_cref  = make_any_input<const type&>(value);

    SECTION("Ctors") {
        SECTION("Default Ctor") {
            REQUIRE_FALSE(defaulted.has_value());
            REQUIRE(defaulted.type() == rtti_type(typeid(nullptr)));
        }

        SECTION("Value Ctor") {
            REQUIRE(by_value.has_value());
            REQUIRE(by_value.type() == rtti_type(typeid(type)));
            REQUIRE(any_cast<type>(by_value) == value);

            REQUIRE(by_cval.has_value());
            REQUIRE(by_cval.type() == rtti_type(typeid(type)));
            REQUIRE(any_cast<const type>(by_cval) == value);

            REQUIRE(by_cref.has_value());
            REQUIRE(by_cref.type() == rtti_type(typeid(type)));
            REQUIRE(&any_cast<const type&>(by_cref) == &value);
        }

        SECTION("Copy Ctor") {
            input_type default_copy(defaulted);
            REQUIRE(default_copy.are_equal(defaulted));

            input_type val_copy(by_value);
            REQUIRE(val_copy.are_equal(by_value));

            input_type cval_copy(by_cval);
            REQUIRE(cval_copy.are_equal(by_cval));

            input_type cref_copy(by_cref);
            REQUIRE(cref_copy.are_equal(by_cref));

            // Copy aliases the same object (doesn't deep copy it)
            REQUIRE(&any_cast<const type&>(cref_copy) == &value);
        }

        SECTION("Move Ctor") {
            input_type default_copy(defaulted);
            input_type default_move(std::move(defaulted));
            REQUIRE(default_copy.are_equal(default_move));

            input_type val_copy(by_value);
            input_type val_move(std::move(by_value));
            REQUIRE(val_copy.are_equal(val_move));

            input_type cval_copy(by_cval);
            input_type cval_move(std::move(by_cval));
            REQUIRE(cval_copy.are_equal(cval_move));

            input_type cref_copy(by_cref);
            input_type cref_move(std::move(by_cref));
            REQUIRE(cref_copy.are_equal(cref_move));

            // Still aliases the same object (doesn't deep copy it)
            REQUIRE(&any_cast<const type&>(cref_move) == &value);
        }

        SECTION("Copy Assignment") {
            input_type default_copy;
            auto p = &(default_copy = defaulted);
            REQUIRE(p == &default_copy);
            REQUIRE(default_copy.are_equal(defaulted));

            input_type val_copy;
            p = &(val_copy = by_value);
            REQUIRE(p == &val_copy);
            REQUIRE(val_copy.are_equal(by_value));

            input_type cval_copy;
            p = &(cval_copy = by_cval);
            REQUIRE(p == &cval_copy);
            REQUIRE(cval_copy.are_equal(by_cval));

            input_type cref_copy;
            p = &(cref_copy = by_cref);
            REQUIRE(cref_copy.are_equal(by_cref));

            // Copy aliases the same object (doesn't deep copy it)
            REQUIRE(&any_cast<const type&>(cref_copy) == &value);
        }

        SECTION("Move Ctor") {
            input_type default_copy(defaulted);
            input_type default_move;
            auto p = &(default_move = std::move(defaulted));
            REQUIRE(p == &default_move);
            REQUIRE(default_copy.are_equal(default_move));

            input_type val_copy(by_value);
            input_type val_move;
            p = &(val_move = std::move(by_value));
            REQUIRE(p == &val_move);
            REQUIRE(val_copy.are_equal(val_move));

            input_type cval_copy(by_cval);
            input_type cval_move;
            p = &(cval_move = std::move(by_cval));
            REQUIRE(p == &cval_move);
            REQUIRE(cval_copy.are_equal(cval_move));

            input_type cref_copy(by_cref);
            input_type cref_move;
            p = &(cref_move = std::move(by_cref));
            REQUIRE(p == &cref_move);
            REQUIRE(cref_copy.are_equal(cref_move));

            // Still aliases the same object (doesn't deep copy it)
            REQUIRE(&any_cast<const type&>(cref_move) == &value);
        }
    }

    SECTION("swap") {
        defaulted.swap(by_value);
        REQUIRE(by_value.are_equal(AnyInput{}));
        REQUIRE(defaulted.are_equal(make_any_input<type>(value)));

        defaulted.swap(by_cval);
        REQUIRE(defaulted.are_equal(make_any_input<const type>(value)));
        REQUIRE(by_cval.are_equal(make_any_input<type>(value)));

        defaulted.swap(by_cref);
        REQUIRE(defaulted.are_equal(make_any_input<const type&>(value)));
        REQUIRE(by_cref.are_equal(make_any_input<const type>(value)));
    }

    SECTION("hash") {
        auto h_default = hash_objects(defaulted);
        auto h_value   = hash_objects(by_value);
        auto h_cval    = hash_objects(by_cval);
        auto h_cref    = hash_objects(by_cref);

        // Defaults are equal
        auto h_default2 = hash_objects(input_type{});
        REQUIRE(h_default == h_default2);

        // Values are equal
        auto h_value2 = hash_objects(make_any_input<type>(value));
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
}
