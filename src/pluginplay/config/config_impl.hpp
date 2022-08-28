#pragma once

namespace pluginplay {

/// Used throughout PluginPlay to enable/disable RocksDB support at compiletime
#ifdef BUILD_ROCKS_DB
static constexpr bool with_rocksdb_v = true;
#else
static constexpr bool with_rocksdb_v = false;
#endif

} // namespace pluginplay
