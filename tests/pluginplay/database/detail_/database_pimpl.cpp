#include <catch2/catch.hpp>
#include <pluginplay/database/detail_/database_pimpl.hpp>

/* Testing Strategy:
 *
 * For the most part DatabasePIMPL only defines an API. Classes which derive
 * from DatabasePIMPL should test their implementations through the API of
 * DatabasePIMPL to ensure they work. The exception to this is the `at` method,
 * which relies on count to determine if a key exists before retrieving the
 * value. If count_ and at_ work, then we only need to test that the logic in
 * at is setup correctly so that it throws when a value isn't found.
 */

TEST_CASE("DatabasePIMPL") {
    // TODO: test DatabasePIMPL::at
}