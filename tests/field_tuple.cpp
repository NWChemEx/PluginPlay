#include <catch2/catch.hpp>
#include <sde/field_tuple.hpp>
#include <sde/module_input.hpp>
#include <sde/module_result.hpp>

using namespace sde;
using namespace sde::detail_;


using PropertyTypeInputBuilder =
  sde::FieldTuple<sde::ModuleInput>;

template<typename T>
void check_state(T& builder, std::vector<std::string> corr_keys) {
    REQUIRE(builder.size() == corr_keys.size());
    auto keyi = corr_keys.begin();
    for(auto[k, v] : builder) {
        REQUIRE(k == *keyi++);
        REQUIRE(builder.count(k));
    }
}

/*
 * We know FieldTupleTraits works from its unit test. This unit test ensures
 * that we forward the types correctly through the FieldTuple class. In other
 * words we need to check we obtain the correct traits_type for various template
 * parameter combinations and then for one of those combinations we need to make
 * sure that the types are forwarded correctly.
 */
TEST_CASE("FieldTuple : types") {

    SECTION("FieldTuple<ModuleInput>::traits_type"){
        using traits_type = typename FieldTuple<ModuleInput>::traits_type;
        using corr = detail_::FieldTupleTraits<ModuleInput>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("FieldTuple<ModuleInput, int>::traits_type"){
        using traits_type = typename FieldTuple<ModuleInput, int>::traits_type;
        using corr = detail_::FieldTupleTraits<ModuleInput, int>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("FieldTuple<ModuleInput, int, const int&>::traits_type"){
        using traits_type =
          typename FieldTuple<ModuleInput, int, const int&>::traits_type;
        using corr = detail_::FieldTupleTraits<ModuleInput, int, const int&>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("FieldTuple<ModuleResult>::traits_type"){
        using traits_type = typename FieldTuple<ModuleResult>::traits_type;
        using corr = detail_::FieldTupleTraits<ModuleResult>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("FieldTuple<ModuleResult, int>::traits_type"){
        using traits_type = typename FieldTuple<ModuleResult, int>::traits_type;
        using corr = detail_::FieldTupleTraits<ModuleResult, int>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("FieldTuple<ModuleResult, int, double>::traits_type"){
        using traits_type =
          typename FieldTuple<ModuleResult, int, double>::traits_type;
        using corr = detail_::FieldTupleTraits<ModuleResult, int, double>;
        STATIC_REQUIRE(std::is_same_v<traits_type, corr>);
    }

    SECTION("Type forwarding"){
        using field_tuple = FieldTuple<ModuleInput, int>;
        using traits_type = typename field_tuple::traits_type;

        SECTION("key_type") {
            using key_type = typename field_tuple::key_type;
            using corr     = typename traits_type::key_type;
            STATIC_REQUIRE(std::is_same_v<key_type, corr>);
        }

        SECTION("mapped_type") {
            using mapped_type = typename field_tuple::mapped_type;
            using corr     = typename traits_type::mapped_type;
            STATIC_REQUIRE(std::is_same_v<mapped_type, corr>);
        }

        SECTION("value_type") {
            using value_type = typename field_tuple::value_type;
            using corr     = typename traits_type::value_type;
            STATIC_REQUIRE(std::is_same_v<value_type, corr>);
        }

        SECTION("iterator") {
            using iterator = typename field_tuple::iterator;
            using corr     = typename traits_type::iterator;
            STATIC_REQUIRE(std::is_same_v<iterator, corr>);
        }

        SECTION("const_iterator") {
            using const_iterator = typename field_tuple::const_iterator;
            using corr           = typename traits_type::const_iterator;
            STATIC_REQUIRE(std::is_same_v<const_iterator, corr>);
        }
    }
}

TEST_CASE("FieldTuple : Default ctor") {
    std::vector<std::string> keys;
    PropertyTypeInputBuilder builder;
    check_state(builder, keys);
    using corr_type = std::tuple<>;
    using fields    = typename decltype(builder)::traits_type::tuple_of_fields;
    STATIC_REQUIRE(std::is_same_v<corr_type, fields>);
}

TEST_CASE("FieldTuple : add_field") {
    PropertyTypeInputBuilder builder;
    std::vector<std::string> keys{"key1"};
    auto new_builder = builder.add_field<int>("key1");
    SECTION("Resulting state") {
        check_state(new_builder, keys);
        using corr_type = std::tuple<int>;
        using fields    = typename decltype(new_builder)::traits_type::tuple_of_fields;
        STATIC_REQUIRE(std::is_same_v<corr_type, fields>);
    }
    SECTION("Can't reuse the same key") {
        REQUIRE_THROWS_AS(new_builder.add_field<int>("key1"),
                          std::invalid_argument);
    }

    SECTION("Can get field") {
        sde::ModuleInput input;
        input.set_type<int>();
        REQUIRE(new_builder.at("key1") == input);
    }

    SECTION("Can add second field") {
        auto newer_builder = new_builder.add_field<double>("key2");
        keys.emplace_back("key2");
        check_state(newer_builder, keys);
    }
}
