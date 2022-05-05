#include <catch2/catch.hpp>

/* This unit test ensures that attempting to create a RocksDBPIMPL fails when
 * RocksDB support has not been enabled. N.B. if RocksDB support is enabled this
 * test will find the real definitions, even though we don't include the real
 * header file. This is because the PluginPlay binary contains the real
 * definitions of the symbols, and (at least with GCC) the definitions in the
 * binary are preferred over the inline definitions in the header file, i.e.,
 * if RocksDB is enabled, then even though we don't include the "real" header
 * file, because of how C++ works, this test will still fail because it still
 * finds the real definitions.
 */
#ifndef BUILD_ROCKS_DB
#include <pluginplay/cache/database/rocksdb/detail_/rocksdb_pimpl_stub.hpp>

using namespace pluginplay::cache::database::detail_;

TEST_CASE("RocksDBPIMPL Stub") {
    REQUIRE_THROWS_AS(RocksDBPIMPL("Not/a/path"), std::runtime_error);
}
#endif
