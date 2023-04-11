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

#include "lexical_cast.hpp"
#include "test_cache.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <pluginplay/any/any.hpp>
#include <pluginplay/cache/database/native.hpp>
#include <pluginplay/cache/uuid_mapper.hpp>
#include <pluginplay/types.hpp>

using namespace pluginplay::cache;
using namespace pluginplay::cache::database;

TEMPLATE_LIST_TEST_CASE("UUIDMapper", "", testing::test_types) {
    using key_type     = TestType;
    using uuid_db_type = UUIDMapper<key_type>;
    using uuid_type    = typename uuid_db_type::mapped_type;
    using key_set_type = typename uuid_db_type::key_set_type;

    auto [pinner, db] = testing::make_nested_native<key_type, uuid_type>();

    key_type key0;
    auto key1 = testing::lexical_cast<key_type>("42");

    uuid_db_type uuid_db(std::make_unique<decltype(db)>(std::move(db)));
    uuid_db.insert(key0);

    SECTION("CTor") {
        REQUIRE_THROWS_AS(uuid_db_type(nullptr), std::runtime_error);
    }

    SECTION("keys") { REQUIRE(uuid_db.keys() == key_set_type{key0}); }

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

/* Acceptance test for UUIDMapper
 *
 * When we use UUIDMapper instances in PluginPlay we always use them as the
 * UUIDMapper<AnyField> specialization. This unit test ensures that when used
 * this way everything works as intended.
 */
TEST_CASE("UUIDMapper<AnyField>") {
    using namespace pluginplay::any;

    using any_type     = pluginplay::type::any;
    using uuid_db_type = UUIDMapper<any_type>;
    using uuid_type    = typename uuid_db_type::mapped_type;

    // N.B. This makes a transposer which wraps a nested naive. In production we
    //      use a transposer which wraps a naive, which wraps a serialized
    auto [psub_sub_sub, psub_sub, sub] =
      testing::make_transposer<any_type, uuid_type>();

    auto sub_db = std::make_unique<decltype(sub)>(std::move(sub));
    auto psub   = sub_db.get();
    uuid_db_type db(std::move(sub_db));

    SECTION("Wrap a double by const ref") {
        using wrapped_type = const double&;
        double x           = 3.14;
        auto te_x          = make_any_field<wrapped_type>(x);
        auto px            = &any_cast<wrapped_type>(te_x);

        // Sanity checks that it's wrapped by const ref
        REQUIRE(te_x.has_value());
        REQUIRE_FALSE(te_x.owns_value());
        REQUIRE(px == &x);

        SECTION("Added by moving") {
            db.insert(std::move(te_x));
            auto other_te_x = make_any_field<wrapped_type>(x);
            REQUIRE(db.count(other_te_x));
            auto uuid = db.at(other_te_x);

            auto held_x = psub_sub->at(uuid.get());

            // Should have been copied
            REQUIRE(held_x.get().has_value());
            REQUIRE_FALSE(held_x.get().owns_value());
            REQUIRE_FALSE(&any_cast<wrapped_type>(held_x.get()) != px);
        }

        SECTION("Add by copy") {
            db.insert(te_x);

            // For good measure set original te_x is out of
            double x2 = 1.23;
            te_x      = make_any_field<wrapped_type>(x2);

            auto other_te_x = make_any_field<wrapped_type>(x);
            REQUIRE(db.count(other_te_x));
            auto uuid = db.at(other_te_x);

            auto held_x = psub_sub->at(uuid.get());

            // Should have been copied
            REQUIRE(held_x.get().has_value());
            REQUIRE(held_x.get().owns_value());
            REQUIRE(&any_cast<wrapped_type>(held_x.get()) != px);
        }
    }

    SECTION("Wrap a std::vector<double> by const ref") {
        using wrapped_type = const std::vector<double>&;
        std::vector<double> x{1.23, 3.14};
        auto p_sub_x = x.data();
        auto te_x    = make_any_field<wrapped_type>(x);
        auto px      = &any_cast<wrapped_type>(te_x);

        // Sanity checks that it's wrapped by const ref
        REQUIRE(te_x.has_value());
        REQUIRE_FALSE(te_x.owns_value());
        REQUIRE(px == &x);
        REQUIRE(px->data() == p_sub_x);

        SECTION("Added by moving") {
            db.insert(std::move(te_x));
            auto other_te_x = make_any_field<wrapped_type>(x);
            REQUIRE(db.count(other_te_x));
            auto uuid = db.at(other_te_x);

            auto held_x = psub_sub->at(uuid.get());

            // Should have been copied
            REQUIRE(held_x.get().has_value());
            REQUIRE_FALSE(held_x.get().owns_value());
            REQUIRE_FALSE(&any_cast<wrapped_type>(held_x.get()) != px);
            REQUIRE_FALSE(any_cast<wrapped_type>(held_x.get()).data() !=
                          p_sub_x);
        }

        SECTION("Add by copy") {
            db.insert(te_x);

            // For good measure set original te_x is out of
            std::vector<double> x2{6.626, 9.87};
            te_x = make_any_field<wrapped_type>(x2);

            auto other_te_x = make_any_field<wrapped_type>(x);
            REQUIRE(db.count(other_te_x));
            auto uuid = db.at(other_te_x);

            auto held_x = psub_sub->at(uuid.get());

            // Should have been copied
            REQUIRE(held_x.get().has_value());
            REQUIRE(held_x.get().owns_value());
            REQUIRE(&any_cast<wrapped_type>(held_x.get()) != px);
            REQUIRE(any_cast<wrapped_type>(held_x.get()).data() != p_sub_x);
        }
    }
}
