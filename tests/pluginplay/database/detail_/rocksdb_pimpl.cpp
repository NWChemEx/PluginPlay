#ifdef BUILD_ROCKS_DB
#include <catch2/catch.hpp>
#include <pluginplay/database/detail_/rocksdb_pimpl.hpp>
using namespace pluginplay::database::detail_;

TEST_CASE("RocksDBPIMPL") {
    // There are concerns about storing values large than 4 GB in a RocksDB
    // instance, this variable can be used to enable unit tests which will
    // verify that large values work. Obviously it uses a large amount of memory
    // so it should only be turned on if you've got the memory/time.
    const bool do_large_value = false;

    RocksDBPIMPL db("test.db");
    db.insert("Hello", "World");
    auto x = db.at("Hello");
    REQUIRE(x.get() == "World");

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
        srand(time(NULL));
        while(buffer.size() < 5E9) { buffer += std::to_string(rand()); }
        db.insert("large value", buffer);
        auto large_value = db.at("large value");
        REQUIRE(large_value.get() == buffer);
    }
}
#endif
