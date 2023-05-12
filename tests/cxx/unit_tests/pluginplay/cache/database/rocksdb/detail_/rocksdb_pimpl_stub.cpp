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

#include <catch2/catch.hpp>

/* This unit test ensures that attempting to create a RocksDBPIMPL fails when
 * RocksDB support has not been enabled.
 *
 * N.B. if RocksDB support is enabled this
 * test will find the real definitions, even though we don't include the real
 * header file. This is because the PluginPlay binary contains the real
 * definitions of the symbols, and (at least with GCC) the definitions in the
 * binary are preferred over the inline definitions in the header file, i.e.,
 * if RocksDB is enabled, then even though we don't include the "real" header
 * file, because of how C++ works, this test will still fail because it still
 * finds the real definitions.
 */
#include <pluginplay/cache/database/rocksdb/detail_/rocksdb_pimpl_stub.hpp>
#include <pluginplay/config/config_impl.hpp>
using namespace pluginplay::cache::database::detail_;

TEST_CASE("RocksDBPIMPL Stub") {
    if constexpr(!pluginplay::with_rocksdb_v) {
        REQUIRE_THROWS_AS(RocksDBPIMPLStub("Not/a/path"), std::runtime_error);
    }
}
