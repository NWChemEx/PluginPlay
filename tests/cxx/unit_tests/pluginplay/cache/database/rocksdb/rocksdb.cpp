/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <catch2/catch.hpp>
#include <filesystem>
#include <pluginplay/cache/database/rocksdb/rocksdb.hpp>
using namespace pluginplay::cache::database;

#ifdef BUILD_ROCKS_DB

/* Testing notes:
 *
 * Warning!!! The database is not purged between sections since its state is
 *            persistant on disk. Care needs to be taken when writing the unit
 *            tests to not rely on sections to undo behavior.
 */

TEST_CASE("RocksDB") {
    std::filesystem::path file("test.db");
    auto p = std::filesystem::temp_directory_path() / file;

    using RocksDBSS = RocksDB<std::string, std::string>;
    RocksDBSS defaulted;
    RocksDBSS db(p.string());
    db.insert("Hello", "World");

    SECTION("CTor") { REQUIRE(std::filesystem::exists(p)); }

    SECTION("count") {
        REQUIRE_FALSE(defaulted.count("not a key"));
        REQUIRE_FALSE(db.count("not a key"));
        REQUIRE(db.count("Hello"));
    }

    SECTION("insert/operator[]") {
        // One insert, nothing special
        REQUIRE(db.at("Hello").get() == "World");

        // Repeated inserts do nothing
        db.insert("Hello", "World");
        auto val = db["Hello"];
        REQUIRE(val.get() == "World");

        // Can be used to override a value
        db.insert("Hello", "Universe");
        auto val2 = db["Hello"];
        REQUIRE(val2.get() == "Universe");

        REQUIRE_THROWS_AS(db.at("Not a key"), std::out_of_range);
        REQUIRE_THROWS_AS(defaulted.insert("", ""), std::runtime_error);
        REQUIRE_THROWS_AS(defaulted[""], std::runtime_error);
    }

    SECTION("free") {
        // Can delete an existing key
        REQUIRE(db.count("Hello"));
        db.free("Hello");
        REQUIRE_FALSE(db.count("Hello"));

        // Can delete a non-existing key
        db.free("Hello");
        REQUIRE_FALSE(db.count("Hello"));

        REQUIRE_THROWS_AS(defaulted.free(""), std::runtime_error);
    }

    SECTION("backup") {}

    SECTION("dump") {}
}
#else

TEST_CASE("RocksDB") {
    using RocksDBSS = RocksDB<std::string, std::string>;
    REQUIRE_THROWS_AS(RocksDBSS("Not/a/path"), std::runtime_error);
}

#endif
