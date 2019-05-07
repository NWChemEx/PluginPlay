#include <catch2/catch.hpp>
#include <sde/detail_/property_type_builder.hpp>
#include <sde/module_input.hpp>

using PropertyTypeInputBuilder =
  sde::detail_::PropertyTypeBuilder<sde::ModuleInput>;

template<typename T>
void check_state(T& builder, std::vector<std::string> corr_keys) {
    REQUIRE(builder.size() == corr_keys.size());
    auto keyi = corr_keys.begin();
    for(auto[k, v] : builder) {
        REQUIRE(k == *keyi++);
        REQUIRE(builder.count(k));
    }
}

TEST_CASE("PropertyTypeBuilder : Default ctor") {
    std::vector<std::string> keys;
    PropertyTypeInputBuilder builder;
    check_state(builder, keys);
    using corr_type = std::tuple<>;
    using fields    = typename decltype(builder)::tuple_of_fields;
    STATIC_REQUIRE(std::is_same_v<corr_type, fields>);
}

TEST_CASE("PropertyTypeBuilder : add_field") {
    PropertyTypeInputBuilder builder;
    std::vector<std::string> keys{"key1"};
    auto new_builder = builder.add_field<int>("key1");
    SECTION("Resulting state") {
        check_state(new_builder, keys);
        using corr_type = std::tuple<int>;
        using fields    = typename decltype(new_builder)::tuple_of_fields;
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
