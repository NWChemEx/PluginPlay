#include "../lexical_cast.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <pluginplay/database/detail_/native.hpp>
#include <pluginplay/database/detail_/uuid_mapper.hpp>

using namespace pluginplay::database::detail_;

using test_types = std::tuple<int, char, std::string>;

TEMPLATE_LIST_TEST_CASE("UUIDMapper", "", test_types) {
    using key_type     = TestType;
    using uuid_db_type = UUIDMapper<key_type>;
    using uuid_type    = typename uuid_db_type::mapped_type;
    using db_type      = Native<key_type, uuid_type>;

    auto inner_db = std::make_unique<db_type>();
    auto pinner   = inner_db.get();
    auto db       = std::make_unique<db_type>(std::move(inner_db));

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
