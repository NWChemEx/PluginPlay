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

#ifdef BUILD_ROCKS_DB
#include <catch2/catch.hpp>
#include <filesystem>
#include <pluginplay/cache/database/rocksdb/detail_/rocksdb_pimpl.hpp>
using namespace pluginplay::cache::database::detail_;

/* Testing notes:
 *
 * Warning!!! The database is not purged between sections since its state is
 *            persistant on disk. Care needs to be taken when writing the unit
 *            tests to not rely on sections to undo behavior.
 */

TEST_CASE("RocksDBPIMPL") {
    // There are concerns about storing values large than 4 GB in a RocksDB
    // instance, this variable can be used to enable unit tests which will
    // verify that large values work. Obviously it uses a large amount of memory
    // so it should only be turned on if you've got the memory/time.
    //
    // FWIW the large value will contain 5E9 characters (so it should be about
    // 5GB). The test stores a copy of it, then retrieves the copy, and finally
    // compares the copy to the original. This means minimally the test needs
    // about 10 GB of memory (watching top it looks it's more like 15 to 20 GB).
    const bool do_large_value = false;

    std::filesystem::path file("test.db");
    auto p = std::filesystem::temp_directory_path() / file;

    RocksDBPIMPL db(p.string());
    db.insert("Hello", "World");

    SECTION("CTor") { REQUIRE(std::filesystem::exists(p)); }

    SECTION("count") {
        REQUIRE_FALSE(db.count("not a key"));
        REQUIRE(db.count("Hello"));
    }

    SECTION("insert/at") {
        // One insert, nothing special
        REQUIRE(db.at("Hello").get() == "World");

        // Repeated inserts do nothing
        db.insert("Hello", "World");
        auto val = db.at("Hello");
        REQUIRE(val.get() == "World");

        // Can be used to override a value
        db.insert("Hello", "Universe");
        auto val2 = db.at("Hello");
        REQUIRE(val2.get() == "Universe");

        // Returns an empty object if key DNE
        REQUIRE_FALSE(db.at("Not a key").has_value());
    }

    SECTION("free") {
        // Can delete an existing key
        REQUIRE(db.count("Hello"));
        db.free("Hello");
        REQUIRE_FALSE(db.count("Hello"));

        // Can delete a non-existing key
        db.free("Hello");
        REQUIRE_FALSE(db.count("Hello"));
    }

    if(do_large_value) {
        // We note that the problem presumably comes from using 32 bit integers,
        // which themselves can only express offsets of 2^2 * 2^30, where 2^30
        // is noteworthy as that's the size of 1 GiB (N.B. GiB > GB, the latter
        // is defined as exactly 10^9 bytes whereas the former is the first
        // power of 2 >= 10^9). Anyways, since a char in C++ is 1 byte, and
        // since std::string is just a container filled with char's, "alls" we
        // need to do for this test is put more than 2^32 characters into a
        // string and try to add it to RocksDB. The quotes on "alls" are because
        // RocksDB may actually do some compression making the value smaller.
        // For good measure we go for 5GB of randomly generated data and assume
        // the compression can't do anything with that.

        std::string buffer;
        REQUIRE(buffer.max_size() > 5E9); // Make sure the test actually can run

        // This fills buffer with random numbers (as strings) until we have more
        // than 5E9 characters in buffer
        srand(time(NULL));
        while(buffer.size() < 5E9) { buffer += std::to_string(rand()); }

        // Now insert a copy of buffer
        db.insert("large value", buffer);

        // Get the copy of the large value back and compare it to buffer
        auto val = db.at("large value");
        REQUIRE(val.get() == buffer);
    }
}
#endif
