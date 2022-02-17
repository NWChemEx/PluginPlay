#include "pluginplay/any/any.hpp"
#include "test_any.hpp"

using namespace pluginplay;

/* Testing Strategy:
 *
 * The majority of make_any_result/make_any_input is taken care of by the ctors
 * for MakeAnyResultWrapper/MakeAnyInputWrapper for good measure we exhaustively
 * test cv and reference-qualified types for each to ensure that the types are
 * forwarded correctly. For simplicity we also test unwrapping the values, via
 * pluginplay::any_cast at the same time (otherwise we'd duplicate the
 * construction for the any_cast tests)
 */

TEMPLATE_LIST_TEST_CASE("make_any_result", "", testing::types2test) {
    using type    = TestType;
    using error_t = std::runtime_error;

    auto value = testing::non_default_value<type>();

    auto has_value = make_any_result<type>(value);

    //
    // The following four tests should raise static asserts (uncomment to test)
    //

    // REQUIRE_THROWS_AS(make_any_result<const type>(value), error_t);
    // REQUIRE_THROWS_AS(make_any_result<type&>(value), error_t);
    // REQUIRE_THROWS_AS(make_any_result<const type&>(value), error_t);
    // REQUIRE_THROWS_AS(any_cast<type&>(has_value), error_t);

    // Can get the value back by copy
    REQUIRE(any_cast<type>(has_value) == value);

    // Can get the value back by read-only copy
    REQUIRE(any_cast<const type>(has_value) == value);

    // Can get the value back by read-only reference
    REQUIRE(any_cast<const type&>(has_value) == value);

    // Throws if wrapped value isn't convertible to requested type
    REQUIRE_THROWS_AS(any_cast<error_t>(has_value), error_t);

    // Throws if any result is empty
    REQUIRE_THROWS_AS(any_cast<type>(AnyResult{}), error_t);

    // Ensures that we actually move the value into the AnyResult and don't
    // copy it.
    if constexpr(std::is_same_v<type, std::vector<int>>) {
        const auto* p = value.data();
        auto moved    = make_any_result<type>(std::move(value));
        REQUIRE(p == any_cast<const type&>(moved).data());
    }
}
