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

namespace pluginplay {

/// Used throughout PluginPlay to enable/disable RocksDB support at compiletime
#ifdef BUILD_ROCKS_DB
static constexpr bool with_rocksdb_v = true;
#else
static constexpr bool with_rocksdb_v = false;
#endif

/// Used throughout PluginPlay to enable/disable pybind11 support at compiletime
#ifdef BUILD_PYBIND11
static constexpr bool with_pybind11_v = true;
#else
static constexpr bool with_pybind11_v = false;
#endif

} // namespace pluginplay
