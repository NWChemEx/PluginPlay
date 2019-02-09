#include <SDE/ModuleInput.hpp>
#include <SDE/detail_/PropertyTypeBuilder.hpp>
#include <catch2/catch.hpp>

using PropertyTypeInputBuilder =
  SDE::detail_::PropertyTypeBuilder<SDE::ModuleInput>;

template<typename T>
void check_state(T& builder, std::vector<std::string> corr_keys) {
    REQUIRE(builder.size() == corr_keys.size());
    auto keyi = corr_keys.begin();
    for(auto[k, v] : builder) {
        REQUIRE(k == *keyi++);
        REQUIRE(builder.count(k));
    }
}

TEST_CASE("PropertyTypeBuilder") {
    std::vector<std::string> keys;

    SECTION("Default Ctor") {
        PropertyTypeInputBuilder builder;
        check_state(builder, keys);
        using corr_type = std::tuple<>;
        using fields    = typename decltype(builder)::tuple_of_fields;
        STATIC_REQUIRE(std::is_same_v<corr_type, fields>);
    }

    PropertyTypeInputBuilder builder;
    keys.emplace_back("key1");

    SECTION("Add field") {
        auto new_builder = builder.add_field<int>("key1");
        check_state(new_builder, keys);
        using corr_type = std::tuple<int>;
        using fields    = typename decltype(new_builder)::tuple_of_fields;
        STATIC_REQUIRE(std::is_same_v<corr_type, fields>);
    }

    auto new_builder = builder.add_field<int>("key1");
    SECTION("Can't reuse the same key") {
        REQUIRE_THROWS_AS(new_builder.add_field<int>("key1"),
                          std::invalid_argument);
    }

    SECTION("Can set field's meta data") {
        new_builder.at("key1").set_description("Hi");
        REQUIRE(new_builder.at("key1").description() == "Hi");
    }

    SECTION("Can add second field") {
        auto newer_builder = new_builder.add_field<double>("key2");
        keys.emplace_back("key2");
        check_state(newer_builder, keys);
    }
}
