/* RocksDB is designed to be an optional dependency. To accomplish this we
 * hide all RocksDB-specific calls in the PIMPL. This file then chooses between
 * the real and the stub PIMPL implementations based on whether or not
 * BUILD_ROCKS_DB is defined. N.B. The stub PIMPL will crash if a user tries to
 * instantiate it.
 */

#ifdef BUILD_ROCKS_DB
#include "rocksdb_pimpl.hpp"
#else
#include "rocksdb_pimpl_stub.hpp"
#endif

namespace pluginplay::database::detail_ {

template class RocksDB<std::string, std::string>;

} // namespace pluginplay::database::detail_
