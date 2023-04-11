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

#pragma once
#include <catch2/catch.hpp>
#include <pluginplay/cache/database/native.hpp>
#include <pluginplay/cache/database/transposer.hpp>
#include <pluginplay/cache/proxy_map_maker.hpp>
#include <pluginplay/cache/uuid_mapper.hpp>
#include <string>
#include <tuple>

namespace testing {

/// Set of types we test as keys
using test_types = std::tuple<int, char, std::string>;

/** @brief Creates a Native DB that backs up to another NativeDB
 *
 *  Native databases have an API DatabaseAPI<key, value> and can optionally
 *  wrap another database of type DatabaseAPI<key, value>. The wrapped database
 *  will be used when backup/dump calls are made (i.e. the wrapped database is
 *  long-term storage). This function wraps the process fo creating a Native
 *  database, `D`, that uses another Native database, `d`, as its long-term
 *  storage. The function will return a pointer to `d` and `D`. N.B. that the
 *  lifetime of `d` is managed by `D` and the pointer is simply for unit-testing
 *  convenience (i.e., to provide direct access to `d`).
 *
 *  @tparam KeyType The type to use for the keys of the resulting database.
 *  @tparam ValueType
 *
 *  @return A tuple such that the zeroth element is a pointer `d` and the first
 *          element is `D`. See full description for definitions of `D` and `d`.
 *
 */
template<typename KeyType, typename ValueType>
auto make_nested_native() {
    // Type of both the outermost and wrapped db
    using db_type = pluginplay::cache::database::Native<KeyType, ValueType>;

    // Will be `d`, i.e., the nested database
    auto sub_db = std::make_unique<db_type>();
    auto p      = sub_db.get();

    db_type rv(std::move(sub_db));
    return std::make_tuple(p, std::move(rv));
}

template<typename KeyType>
auto make_uuid_mapper() {
    using uuid_mapper_type = pluginplay::cache::UUIDMapper<KeyType>;
    using uuid_type        = typename uuid_mapper_type::mapped_type;
    auto [p, db]           = make_nested_native<KeyType, uuid_type>();
    auto wrapped_db        = std::make_unique<decltype(db)>(std::move(db));
    auto pdb               = wrapped_db.get();
    uuid_mapper_type rv(std::move(wrapped_db));
    return std::make_tuple(p, pdb, std::move(rv));
}

template<typename KeyType>
auto make_proxy_map_maker() {
    using key_mapped_type  = typename KeyType::mapped_type;
    auto [p, pdb, uuid_db] = make_uuid_mapper<key_mapped_type>();
    using uuid_mapper_type = decltype(uuid_db);
    auto wrapped_db = std::make_unique<uuid_mapper_type>(std::move(uuid_db));
    auto puuid_db   = wrapped_db.get();
    pluginplay::cache::ProxyMapMaker<KeyType> rv(std::move(wrapped_db));
    return std::make_tuple(p, pdb, puuid_db, std::move(rv));
}

template<typename KeyType, typename ValueType>
auto make_transposer() {
    auto [psub_sub_db, sub_db] = make_nested_native<ValueType, KeyType>();
    auto wrapped_db = std::make_unique<decltype(sub_db)>(std::move(sub_db));
    auto psub_db    = wrapped_db.get();
    using namespace pluginplay::cache::database;
    Transposer<KeyType, ValueType> rv(std::move(wrapped_db));
    return std::make_tuple(psub_sub_db, psub_db, std::move(rv));
}

} // namespace testing
