#include <catch2/catch.hpp>
#include <filesystem>
#include <pluginplay/cache/database/database_factory.hpp>

using namespace pluginplay;
using namespace pluginplay::cache::database;

/* These tests serve as integration tests for the Database backend of the cache.
 * It is assumed that the pieces of the Database have been individually unit
 * tested, and found to work. The tests here focus on using the objects returned
 * by DatabaseFactory instances when:
 *
 * - there is no long-term storage
 * - there is long-term storage, but a fresh initialization
 * - there is long-term storage, and we're reading things in.
 */

TEST_CASE("DatabaseFactory") {}
