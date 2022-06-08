#include "lexical_cast.hpp"
#include "test_cache.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <pluginplay/cache/database/native.hpp>
#include <pluginplay/cache/uuid_mapper.hpp>
#include <pluginplay/types.hpp>

using namespace pluginplay::cache;
using namespace pluginplay::cache::database;

TEMPLATE_LIST_TEST_CASE("UUIDMapper", "", testing::test_types) {
    using key_type     = TestType;
    using uuid_db_type = UUIDMapper<key_type>;
    using uuid_type    = typename uuid_db_type::mapped_type;

    auto [pinner, db] = testing::make_nested_native<key_type, uuid_type>();

    key_type key0;
    auto key1 = testing::lexical_cast<key_type>("42");

    uuid_db_type uuid_db(std::move(db));
    uuid_db.insert(key0);

    SECTION("CTor") {
        REQUIRE_THROWS_AS(uuid_db_type(nullptr), std::runtime_error);
    }

    SECTION("count") {
        REQUIRE(uuid_db.count(key0));
        REQUIRE_FALSE(uuid_db.count(key1));
    }

    SECTION("insert/at") {
        // Note the generated values should be different every time this runs
        uuid_db.insert(key1);

        auto v0 = uuid_db.at(key0).get();

        // Here we just test that different keys get different UUIDs
        REQUIRE(uuid_db.at(key0).get() != uuid_db.at(key1).get());

        // and that calling insert again doesn't generate a new UUID
        uuid_db.insert(key0);
        REQUIRE(v0 == uuid_db.at(key0).get());
    }

    SECTION("free") {
        uuid_db.free(key0);
        REQUIRE_FALSE(uuid_db.count(key0));

        // No-op if key DNE
        uuid_db.free(key1);
        REQUIRE_FALSE(uuid_db.count(key1));
    }

    SECTION("backup") {
        REQUIRE_FALSE(pinner->count(key0));

        uuid_db.backup();
        REQUIRE(uuid_db.count(key0));
        REQUIRE(pinner->count(key0));
    }

    SECTION("dump") {
        REQUIRE_FALSE(pinner->count(key0));

        uuid_db.dump();
        REQUIRE_FALSE(uuid_db.count(key0));
        REQUIRE(pinner->count(key0));
    }
}

TEST_CASE("UUIDMapper<AnyField>") {
    using any_type     = pluginplay::type::any;
    using uuid_db_type = UUIDMapper<any_type>;
    using uuid_type    = typename uuid_db_type::mapped_type;

    auto [psub_sub_sub, psub_sub, sub] =
      testing::make_transposer<any_type, uuid_type>();

    auto sub_db = std::make_unique<decltype(sub)>(std::move(sub));
    auto psub   = sub_db.get();
    uuid_db_type db(std::move(sub_db));
}
