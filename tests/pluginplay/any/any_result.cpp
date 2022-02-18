#include "pluginplay/any/any.hpp"

#include "test_any.hpp"

using namespace pluginplay;

/* Testing Strategy:
 *
 * AnyResult adds result-specific functions to the AnyField class. These unit
 * tests only focus on testing the functions implemented in the AnyResult class.
 * Functions implemented in the AnyField class are unit tested in any_field.cpp.
 * We use make_any_result to create AnyResult instances and any_cast to unwrap
 * them, the tests here assume they have already been tested (they are tested in
 * any.cpp).
 */

TEMPLATE_LIST_TEST_CASE("AnyResult", "", testing::types2test) {
    using type        = TestType;
    using result_type = AnyResult;
    using rtti_type   = typename result_type::rtti_type;

    auto value = testing::non_default_value<type>();

    result_type defaulted;
    auto has_value = make_any_result<type>(value);

    SECTION("CTors") {
        // Check the state of a default constructed instance
        SECTION("Default Ctor") {
            REQUIRE_FALSE(defaulted.has_value());
            REQUIRE(defaulted.type() == rtti_type(typeid(nullptr)));
            REQUIRE_THROWS_AS(any_cast<type>(defaulted), std::runtime_error);
        }

        // Check the state of a value constructed instance
        SECTION("Value Ctor") {
            REQUIRE(has_value.has_value());
            REQUIRE(has_value.type() == rtti_type(typeid(type)));
            REQUIRE(any_cast<type>(has_value) == value);
        }

        SECTION("Copy Ctor") {
            result_type copied_default(defaulted);
            REQUIRE(copied_default.are_equal(defaulted));

            result_type copied_value(has_value);
            REQUIRE(copied_value.are_equal(has_value));

            // This is a check that they are deep copied and not just aliases
            if constexpr(std::is_same_v<type, std::vector<int>>) {
                auto pcopy = any_cast<const type&>(copied_default).data();
                auto pval  = any_cast<const type&>(has_value).data();
                REQUIRE(pcopy != pval);
            }
        }

        SECTION("Move Ctor") {
            result_type copied_default(defaulted);
            result_type moved_default(std::move(defaulted));
            REQUIRE(copied_default.are_equal(moved_default));

            result_type copied_value(has_value);
            result_type moved_value(std::move(has_value));
            REQUIRE(copied_value.are_equal(moved_value));

            // This is a check that they are actually moving and not copying
            if constexpr(std::is_same_v<type, std::vector<int>>) {
                auto pcopy = any_cast<const type&>(copied_default).data();
                result_type moved2(std::move(copied_default));
                auto pval = any_cast<const type&>(moved2).data();
                REQUIRE(pcopy == pval);
            }
        }

        SECTION("Copy Assignment") {
            result_type copied_default;
            auto pcopied = &(copied_default = defaulted);
            REQUIRE(pcopied == &copied_default);
            REQUIRE(copied_default.are_equal(defaulted));

            result_type copied_value;
            pcopied = &(copied_value = has_value);
            REQUIRE(pcopied == &copied_value);
            REQUIRE(copied_value.are_equal(has_value));

            // This is a check that they are deep copied and not just aliases
            if constexpr(std::is_same_v<type, std::vector<int>>) {
                auto pcopy = any_cast<const type&>(copied_default).data();
                auto pval  = any_cast<const type&>(has_value).data();
                REQUIRE(pcopy != pval);
            }
        }

        SECTION("Move assignment") {
            result_type copied_default(defaulted);
            result_type moved_default;
            auto pmoved = &(moved_default = std::move(defaulted));
            REQUIRE(pmoved == &moved_default);
            REQUIRE(copied_default.are_equal(moved_default));

            result_type copied_value(has_value);
            result_type moved_value;
            pmoved = &(moved_value = std::move(has_value));
            REQUIRE(pmoved == &moved_value);
            REQUIRE(copied_value.are_equal(moved_value));

            // This is a check that they are actually moving and not copying
            if constexpr(std::is_same_v<type, std::vector<int>>) {
                auto pcopy = any_cast<const type&>(copied_default).data();
                result_type moved2;
                moved2    = std::move(copied_default);
                auto pval = any_cast<const type&>(moved2).data();
                REQUIRE(pcopy == pval);
            }
        }
    }

    SECTION("swap") {
        defaulted.swap(has_value);
        REQUIRE(has_value.are_equal(result_type{}));
        REQUIRE(defaulted.are_equal(make_any_result<type>(value)));
    }

    SECTION("reset") {
        defaulted.reset();
        REQUIRE_FALSE(defaulted.has_value());

        has_value.reset();
        REQUIRE_FALSE(has_value.has_value());
    }

    SECTION("has_value") {
        // Ensure defaulted instance registers as not having a value
        REQUIRE_FALSE(defaulted.has_value());

        // Ensure value-wrapping instance registers as havine a value
        REQUIRE(has_value.has_value());
    }
}
