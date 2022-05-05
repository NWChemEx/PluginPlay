#include <catch2/catch.hpp>
#include <pluginplay/cache/database/db_value.hpp>

using namespace pluginplay::cache::database;

using type_list = std::tuple<int, double>;

TEMPLATE_LIST_TEST_CASE("DBValue", "", type_list) {
    using test_type  = TestType;
    using value_type = DBValue<test_type>;
    using pointer    = typename value_type::pointer;

    SECTION("typedefs") {
        using value_type_in = typename value_type::value_type;
        STATIC_REQUIRE(std::is_same_v<value_type_in, test_type>);

        using reference = typename value_type::reference;
        STATIC_REQUIRE(std::is_same_v<reference, test_type&>);

        STATIC_REQUIRE(std::is_same_v<pointer, test_type*>);

        using const_reference = typename value_type::const_reference;
        STATIC_REQUIRE(std::is_same_v<const_reference, const test_type&>);
    }

    value_type defaulted;
    test_type four{4};
    value_type by_val(test_type{4});
    value_type by_ref(&four);

    SECTION("has_value") {
        REQUIRE_FALSE(defaulted.has_value());
        REQUIRE(by_val.has_value());
        REQUIRE(by_ref.has_value());
    }

    SECTION("get()") {
        REQUIRE(&by_ref.get() == &four);
        REQUIRE(by_val.get() == four);
    }

    SECTION("get()const") {
        REQUIRE(&(std::as_const(by_ref).get()) == &four);
        REQUIRE(std::as_const(by_val).get() == four);
    }
}
