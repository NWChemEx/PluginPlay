#include <catch2/catch.hpp>
#include <iostream>
#include <pluginplay/database/detail_/rocksdb_pimpl.hpp>

using namespace pluginplay::database::detail_;

TEST_CASE("RocksDB") {
    RocksDB<std::string, std::string> db("test.db");
    db.insert("Hello", "World");
    auto x   = db["Hello"];
    auto foo = db["foo"];
    std::cout << foo.get() << std::endl;
    REQUIRE(x.get() == "World");
    std::cout << x.get() << std::endl;
}
