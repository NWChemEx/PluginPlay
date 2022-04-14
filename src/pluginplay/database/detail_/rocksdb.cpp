/* RocksDB is designed to be an optional dependency. To accomplish this we need
 * to use preprocessor macros. Our goal is to localize them as much as possible.
 * To that end, we define two PIMPL implementations, rocksdb_pimpl.hpp contains
 * the real one which gets included when BUILD_ROCKS_DB is defined and
 * rocksdb_pimpl_stub.hpp gets included otherwise. The dispatch between the two
 * happens here (away from the API of the RocksDB class).
 */

#ifdef BUILD_ROCKS_DB
#include "rocksdb_pimpl.hpp"
#else
#include "rocksdb_pimpl_stub.hpp"
#endif

namespace pluginplay::database::detail_ {

template class RocksDB<std::string, std::string>;

} // namespace pluginplay::database::detail_
