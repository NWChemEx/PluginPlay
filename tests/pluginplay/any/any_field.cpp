#include "pluginplay/any/any.hpp"
#include "test_any.hpp"

using namespace pluginplay;

/* Testing Strategy:
 *
 * make_any_input/make_any_result (as well as any_cast) are tested in any.cpp.
 * We simply assume they work here. To actually test AnyField we need to make
 * AnyInput or AnyResult instances because AnyField is an abstract class. The
 * unit tests concentrate purely on the methods which are implemented in the
 * AnyField base class. Methods implemented/defined in the AnyInput or AnyResult
 * classes are tested in any_input.cpp and any_result.cpp respectively. For the
 * purposes of "implemented" we consider a function implemented in AnyField if
 * the method is accessed through AnyFieldBase.
 */

TEMPLATE_LIST_TEST_CASE("AnyField", "", testing::types2test) {
    using type      = TestType;
    using rtti_type = AnyResult::rtti_type;

    // type we use when we want to guarantee a different wrapped type
    // (assumes std::map<int, int> is not in testing::types2test)
    using map_type = std::map<int, int>;

    // The next lines get a non-default value and make some AnyFields

    auto value = testing::non_default_value<type>();
    AnyResult rdefault;
    auto rvalue = make_any_result<type>(value);

    AnyInput idefault;
    auto ivalue = make_any_input<type>(value);
    auto icval  = make_any_input<const type>(value);
    auto icref  = make_any_input<const type&>(value);

    SECTION("type") {
        REQUIRE(rdefault.type() == rtti_type(typeid(nullptr)));
        REQUIRE(rvalue.type() == rtti_type(typeid(type)));

        REQUIRE(idefault.type() == rtti_type(typeid(nullptr)));
        REQUIRE(ivalue.type() == rtti_type(typeid(type)));
        REQUIRE(icval.type() == rtti_type(typeid(type)));
        REQUIRE(icref.type() == rtti_type(typeid(type)));
    }

    SECTION("are_equal") {
        // Defaulted AnyResult instances
        REQUIRE(rdefault.are_equal(AnyResult{}));

        // Non-default and default AnyResult instances
        REQUIRE_FALSE(rdefault.are_equal(rvalue));

        // AnyResult with same wrapped values
        REQUIRE(rvalue.are_equal(make_any_result<type>(value)));

        // AnyResult instances with different wrapped values
        REQUIRE_FALSE(rvalue.are_equal(make_any_result<type>(type{})));

        // AnyResult instances with different wrapped types
        REQUIRE_FALSE(rvalue.are_equal(make_any_result<map_type>(map_type{})));

        // Defaulted AnyResult and defaulted AnyInput
        REQUIRE_FALSE(rdefault.are_equal(idefault));

        // Two default AnyInputs
        REQUIRE(idefault.are_equal(AnyInput{}));

        // AnyInputs with same values
        REQUIRE(ivalue.are_equal(make_any_input<type>(value)));

        // AnyInputs with different values
        REQUIRE_FALSE(ivalue.are_equal(make_any_input<type>(type{})));

        // AnyInputs with same read-only value
        REQUIRE(icval.are_equal(make_any_input<const type>(value)));

        // AnyInputs with same read-only reference
        REQUIRE(icref.are_equal(make_any_input<const type&>(value)));

        // AnyInputs that hold the value differently
        REQUIRE_FALSE(ivalue.are_equal(icval));
        REQUIRE_FALSE(ivalue.are_equal(icref));
        REQUIRE_FALSE(icval.are_equal(icref));

        // AnyInputs with different wrapped types
        REQUIRE_FALSE(ivalue.are_equal(make_any_input<map_type>(map_type{})));
    }

    SECTION("print") {
        std::stringstream ss;

        SECTION("default AnyResult") {
            auto pss = &rdefault.print(ss);
            REQUIRE(pss == &ss); // Returns ss for chaining
            REQUIRE(ss.str() == "");
        }

        SECTION("AnyResult w/ value") {
            auto pss = &rvalue.print(ss);
            REQUIRE(pss == &ss); // Returns ss for chaining

            if constexpr(std::is_same_v<type, int>) {
                REQUIRE(ss.str() == "42");
            } else if constexpr(std::is_same_v<type, double>) {
                REQUIRE(ss.str() == "3.14");
            } else if constexpr(std::is_same_v<type, std::string>) {
                REQUIRE(ss.str() == "Hello World");
            }
        }

        SECTION("default AnyInput") {
            auto pss = &idefault.print(ss);
            REQUIRE(pss == &ss); // Returns ss for chaining
            REQUIRE(ss.str() == "");
        }

        SECTION("AnyInputw/ value") {
            auto pss = &ivalue.print(ss);
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
}
