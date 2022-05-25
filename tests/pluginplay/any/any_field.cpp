#include "pluginplay/any/any.hpp"
#include "test_any.hpp"
#include <map>
#include <sstream>

using namespace pluginplay::any;

/* Testing Strategy:
 *
 * For the most part creation of AnyField is usually done via `make_any_field`.
 * Similarly unwrapping the AnyField can only be done via `any_cast`.
 * `make_any_field` (as well as `any_cast`) are tested in any.cpp. We simply
 * assume they work here.
 */

TEMPLATE_LIST_TEST_CASE("AnyField", "", testing::types2test) {
    using type      = TestType;
    using rtti_type = AnyField::rtti_type;

    // type we use when we want to guarantee a different wrapped type
    // (assumes std::map<int, int> is not in testing::types2test)
    using map_type = std::map<int, int>;

    // The next lines get a non-default value and make some AnyFields
    auto value = testing::non_default_value<type>();

    AnyField defaulted; // Uses default ctor

    /// All three of these go through the value ctor
    auto default_val = make_any_field<type>(type{});
    auto by_value    = make_any_field<type>(value);
    auto by_cval     = make_any_field<const type>(value);
    auto by_cref     = make_any_field<const type&>(value);
    auto diff        = make_any_field<map_type>(map_type{});

    rtti_type rtti(typeid(type));

    SECTION("Ctors") {
        SECTION("Default Ctor") {
            REQUIRE_FALSE(defaulted.has_value());
            REQUIRE(defaulted.type() == rtti_type(typeid(nullptr)));
        }

        SECTION("Value Ctor") {
            REQUIRE(by_value.has_value());
            REQUIRE(by_value.type() == rtti);
            REQUIRE(any_cast<type>(by_value) == value);

            REQUIRE(by_cval.has_value());
            REQUIRE(by_cval.type() == rtti);
            REQUIRE(any_cast<const type>(by_cval) == value);

            REQUIRE(by_cref.has_value());
            REQUIRE(by_cref.type() == rtti);
            REQUIRE(&any_cast<const type&>(by_cref) == &value);
        }

        SECTION("Copy Ctor") {
            AnyField default_copy(defaulted);
            REQUIRE(default_copy == defaulted);

            AnyField val_copy(by_value);
            REQUIRE(val_copy == by_value);

            AnyField cval_copy(by_cval);
            REQUIRE(cval_copy == by_cval);

            AnyField cref_copy(by_cref);
            REQUIRE(cref_copy == by_cref);

            // Deep copies
            REQUIRE(&any_cast<const type&>(cref_copy) != &value);
        }

        SECTION("Move Ctor") {
            AnyField default_copy(defaulted);
            AnyField default_move(std::move(defaulted));
            REQUIRE(default_copy == default_move);

            AnyField val_copy(by_value);
            AnyField val_move(std::move(by_value));
            REQUIRE(val_copy == val_move);

            AnyField cval_copy(by_cval);
            AnyField cval_move(std::move(by_cval));
            REQUIRE(cval_copy == cval_move);

            AnyField cref_copy(by_cref);
            AnyField cref_move(std::move(by_cref));
            REQUIRE(cref_copy == cref_move);

            // Still aliases the same object (doesn't deep copy it)
            REQUIRE(&any_cast<const type&>(cref_move) == &value);
        }

        SECTION("Copy Assignment") {
            AnyField default_copy;
            auto p = &(default_copy = defaulted);
            REQUIRE(p == &default_copy);
            REQUIRE(default_copy == defaulted);

            AnyField val_copy;
            p = &(val_copy = by_value);
            REQUIRE(p == &val_copy);
            REQUIRE(val_copy == by_value);

            AnyField cval_copy;
            p = &(cval_copy = by_cval);
            REQUIRE(p == &cval_copy);
            REQUIRE(cval_copy == by_cval);

            AnyField cref_copy;
            p = &(cref_copy = by_cref);
            REQUIRE(cref_copy == by_cref);

            // Copy is deep copy
            REQUIRE(&any_cast<const type&>(cref_copy) != &value);
        }

        SECTION("Move Assignment") {
            AnyField default_copy(defaulted);
            AnyField default_move;
            auto p = &(default_move = std::move(defaulted));
            REQUIRE(p == &default_move);
            REQUIRE(default_copy == default_move);

            AnyField val_copy(by_value);
            AnyField val_move;
            p = &(val_move = std::move(by_value));
            REQUIRE(p == &val_move);
            REQUIRE(val_copy == val_move);

            AnyField cval_copy(by_cval);
            AnyField cval_move;
            p = &(cval_move = std::move(by_cval));
            REQUIRE(p == &cval_move);
            REQUIRE(cval_copy == cval_move);

            AnyField cref_copy(by_cref);
            AnyField cref_move;
            p = &(cref_move = std::move(by_cref));
            REQUIRE(p == &cref_move);
            REQUIRE(cref_copy == cref_move);

            // Still aliases the same object (doesn't deep copy it)
            REQUIRE(&any_cast<const type&>(cref_move) == &value);
        }
    }

    SECTION("swap") {
        defaulted.swap(by_value);
        REQUIRE(by_value.are_equal(AnyField{}));
        REQUIRE(defaulted.are_equal(make_any_field<type>(value)));

        defaulted.swap(by_cval);
        REQUIRE(defaulted.are_equal(make_any_field<const type>(value)));
        REQUIRE(by_cval.are_equal(make_any_field<type>(value)));

        defaulted.swap(by_cref);
        REQUIRE(defaulted.are_equal(make_any_field<const type&>(value)));
        REQUIRE(by_cref.are_equal(make_any_field<const type>(value)));
    }

    SECTION("type") {
        REQUIRE(defaulted.type() == rtti_type(typeid(nullptr)));
        REQUIRE(by_value.type() == rtti);
        REQUIRE(by_cval.type() == rtti);
        REQUIRE(by_cref.type() == rtti);
    }

    SECTION("operator==/operator!=") {
        // Two default AnyFields
        REQUIRE(defaulted == AnyField{});
        REQUIRE_FALSE(defaulted != AnyField{});

        // AnyFields with same values
        REQUIRE(by_value == make_any_field<type>(value));
        REQUIRE_FALSE(by_value != make_any_field<type>(value));

        // AnyFields with different values
        REQUIRE_FALSE(by_value == default_val);
        REQUIRE(by_value != default_val);

        // AnyFields with same read-only value
        REQUIRE(by_cval == make_any_field<const type>(value));
        REQUIRE_FALSE(by_cval != make_any_field<const type>(value));

        // AnyFields with same read-only reference
        REQUIRE(by_cref == make_any_field<const type&>(value));
        REQUIRE_FALSE(by_cref != make_any_field<const type&>(value));

        // AnyFields that hold the value differently
        REQUIRE(by_value == by_cval);
        REQUIRE_FALSE(by_value != by_cval);
        REQUIRE(by_value == by_cref);
        REQUIRE_FALSE(by_value != by_cref);
        REQUIRE(by_cval == by_cref);
        REQUIRE_FALSE(by_cval != by_cref);

        // AnyFields with different wrapped types
        REQUIRE_FALSE(by_value == diff);
        REQUIRE(by_value != diff);
    }

    SECTION("are_equal") {
        // Two default AnyFields
        REQUIRE(defaulted.are_equal(AnyField{}));

        // AnyFields with same values
        REQUIRE(by_value.are_equal(make_any_field<type>(value)));

        // AnyFields with different values
        REQUIRE_FALSE(by_value.are_equal(default_val));

        // AnyFields with same read-only value
        REQUIRE(by_cval.are_equal(make_any_field<const type>(value)));

        // AnyFields with same read-only reference
        REQUIRE(by_cref.are_equal(make_any_field<const type&>(value)));

        // AnyFields that hold the value differently
        REQUIRE_FALSE(by_value.are_equal(by_cval));
        REQUIRE_FALSE(by_value.are_equal(by_cref));
        REQUIRE_FALSE(by_cval.are_equal(by_cref));

        // AnyFields with different wrapped types
        REQUIRE_FALSE(by_value.are_equal(diff));
    }

    SECTION("print") {
        std::stringstream ss;

        SECTION("default AnyField") {
            auto pss = &defaulted.print(ss);
            REQUIRE(pss == &ss); // Returns ss for chaining
            REQUIRE(ss.str() == "");
        }

        SECTION("AnyField w/ value") {
            auto pss = &by_value.print(ss);
            REQUIRE(pss == &ss); // Returns ss for chaining

            if constexpr(std::is_same_v<type, int>) {
                REQUIRE(ss.str() == "42");
            } else if constexpr(std::is_same_v<type, double>) {
                REQUIRE(ss.str() == "3.14");
            } else if constexpr(std::is_same_v<type, std::string>) {
                REQUIRE(ss.str() == "Hello World");
            }
        }
    }

    SECTION("has_value") {
        REQUIRE_FALSE(defaulted.has_value());
        REQUIRE(by_value.has_value());
        REQUIRE(by_cval.has_value());
        REQUIRE(by_cref.has_value());
    }

    SECTION("owns_value") {
        REQUIRE_FALSE(defaulted.owns_value());
        REQUIRE(by_value.owns_value());
        REQUIRE(by_cval.owns_value());
        REQUIRE_FALSE(by_cref.owns_value());
    }
}
