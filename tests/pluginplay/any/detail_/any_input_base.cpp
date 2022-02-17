#include "pluginplay/any/detail_/any_input_wrapper.hpp"
#include <map>

#include "../test_any.hpp"

using namespace pluginplay::detail_;

/* Testing Strategy:
 *
 * All of the base classes in the any hierarchy are abstract except for the
 * most derived ones. Hence we need to create instances of the most derived
 * classes to test the base classes. In this file we only unit test functions
 * which are defined in the AnyInputBase class and use other files to unit test
 * functions overriden elsewhere in the hierarchy.
 */

TEMPLATE_LIST_TEST_CASE("AnyInputBase", "", testing::types2test) {
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

    SECTION("cast") {
        using error_t = std::runtime_error;

        REQUIRE(defaulted.template cast<type>() == type{});
        REQUIRE(defaulted.template cast<const type>() == type{});
        REQUIRE(defaulted.template cast<type&>() == type{});
        REQUIRE(defaulted.template cast<const type&>() == type{});
        REQUIRE_THROWS_AS(defaulted.template cast<map_type>(), error_t);

        REQUIRE(has_value.template cast<type>() == value);
        REQUIRE(has_value.template cast<const type>() == value);
        REQUIRE(has_value.template cast<type&>() == value);
        REQUIRE(has_value.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(has_value.template cast<map_type>(), error_t);

        REQUIRE(const_val.template cast<type>() == value);
        REQUIRE(const_val.template cast<const type>() == value);
        REQUIRE_THROWS_AS(const_val.template cast<type&>() == value, error_t);
        REQUIRE(const_val.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(const_val.template cast<map_type>(), error_t);

        REQUIRE(const_ref.template cast<type>() == value);
        REQUIRE(const_ref.template cast<const type>() == value);
        REQUIRE_THROWS_AS(const_ref.template cast<type&>() == value, error_t);
        REQUIRE(const_ref.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(const_ref.template cast<map_type>(), error_t);
    }

    SECTION("cast() const") {
        using error_t = std::runtime_error;

        const auto& cdefault = std::as_const(defaulted);
        REQUIRE(cdefault.template cast<type>() == type{});
        REQUIRE(cdefault.template cast<const type>() == type{});
        REQUIRE(cdefault.template cast<const type&>() == type{});
        REQUIRE_THROWS_AS(cdefault.template cast<map_type>(), error_t);

        const auto& cval = std::as_const(has_value);
        REQUIRE(cval.template cast<type>() == value);
        REQUIRE(cval.template cast<const type>() == value);
        REQUIRE(cval.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(cval.template cast<map_type>(), error_t);

        const auto& cval2 = std::as_const(const_val);
        REQUIRE(cval2.template cast<type>() == value);
        REQUIRE(cval2.template cast<const type>() == value);
        REQUIRE(cval2.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(cval2.template cast<map_type>(), error_t);

        const auto& cref = std::as_const(const_ref);
        REQUIRE(cref.template cast<type>() == value);
        REQUIRE(cref.template cast<const type>() == value);
        REQUIRE(cref.template cast<const type&>() == value);
        REQUIRE_THROWS_AS(cref.template cast<map_type>(), error_t);

        // Trying to get a mutable reference back from a read-only AnyInputBase
        // will trip a static assert (and it should). Uncomment this line to
        // check it
        // REQUIRE(cdefault.template cast<type&>() == type{});
    }

    SECTION("is_convertible") {
        REQUIRE(defaulted.template is_convertible<type>());
        REQUIRE(defaulted.template is_convertible<const type>());
        REQUIRE(defaulted.template is_convertible<type&>());
        REQUIRE(defaulted.template is_convertible<const type&>());
        REQUIRE_FALSE(defaulted.template is_convertible<map_type>());

        REQUIRE(has_value.template is_convertible<type>());
        REQUIRE(has_value.template is_convertible<const type>());
        REQUIRE(has_value.template is_convertible<type&>());
        REQUIRE(has_value.template is_convertible<const type&>());
        REQUIRE_FALSE(has_value.template is_convertible<map_type>());

        REQUIRE(const_val.template is_convertible<type>());
        REQUIRE(const_val.template is_convertible<const type>());
        REQUIRE_FALSE(const_val.template is_convertible<type&>());
        REQUIRE(const_val.template is_convertible<const type&>());
        REQUIRE_FALSE(const_val.template is_convertible<map_type>());

        REQUIRE(const_ref.template is_convertible<type>());
        REQUIRE(const_ref.template is_convertible<const type>());
        REQUIRE_FALSE(const_ref.template is_convertible<type&>());
        REQUIRE(const_ref.template is_convertible<const type&>());
        REQUIRE_FALSE(const_ref.template is_convertible<map_type>());
    }

    SECTION("is_convertible() const") {
        const auto& cdefault = std::as_const(defaulted);
        REQUIRE(cdefault.template is_convertible<type>());
        REQUIRE(cdefault.template is_convertible<const type>());
        REQUIRE_FALSE(cdefault.template is_convertible<type&>());
        REQUIRE(cdefault.template is_convertible<const type&>());
        REQUIRE_FALSE(cdefault.template is_convertible<map_type>());

        const auto& cval = std::as_const(has_value);
        REQUIRE(cval.template is_convertible<type>());
        REQUIRE(cval.template is_convertible<const type>());
        REQUIRE_FALSE(cval.template is_convertible<type&>());
        REQUIRE(cval.template is_convertible<const type&>());
        REQUIRE_FALSE(cval.template is_convertible<map_type>());

        const auto& cval2 = std::as_const(const_val);
        REQUIRE(cval2.template is_convertible<type>());
        REQUIRE(cval2.template is_convertible<const type>());
        REQUIRE_FALSE(cval2.template is_convertible<type&>());
        REQUIRE(cval2.template is_convertible<const type&>());
        REQUIRE_FALSE(cval2.template is_convertible<map_type>());

        const auto& cref = std::as_const(const_ref);
        REQUIRE(cref.template is_convertible<type>());
        REQUIRE(cref.template is_convertible<const type>());
        REQUIRE_FALSE(cref.template is_convertible<type&>());
        REQUIRE(cref.template is_convertible<const type&>());
        REQUIRE_FALSE(cref.template is_convertible<map_type>());
    }
}
