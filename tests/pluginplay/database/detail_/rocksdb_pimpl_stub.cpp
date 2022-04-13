#include <catch2/catch.hpp>
#include <pluginplay/database/detail_/rocksdb_pimpl_stub.hpp>

using namespace pluginplay::database::detail_;

TEST_CASE("RocksDBPIMPL Stub") {
    REQUIRE_THROWS_AS(RocksDBPIMPL("Not/a/path"), std::runtime_error);
}