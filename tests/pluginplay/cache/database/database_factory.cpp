#include <catch2/catch.hpp>
#include <filesystem>
#include <pluginplay/cache/database/database_factory.hpp>

using namespace pluginplay;
using namespace pluginplay::cache::database;

// These tests basically just test that the code compiles. Each layer of the
// resulting database is unit tested elsewhere (and works). We leave it up to
// acceptance testing to establish that putting all the pieces together results
// in a database that does what we want.

TEST_CASE("DatabaseFactory") {
    // This basically just tests that it compiles
    std::filesystem::path cache_file("cache.db");
    std::filesystem::path uuid_file("uuid.db");
    auto p = std::filesystem::current_path() / cache_file;
    auto q = std::filesystem::current_path() / uuid_file;

    DatabaseFactory db_factory(p.string(), q.string());
    auto db = db_factory.default_module_db("a_module");
    REQUIRE(db); // Is non-null
}
