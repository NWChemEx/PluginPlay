#include <parallelzone/hasher.hpp>
/** @file hasher.hpp
 *
 * This header injects hashing utilities into pluginplay namespace.
 *
 */

namespace pluginplay {
using parallelzone::hash_objects;
using parallelzone::hash_to_string;
using parallelzone::Hasher;
using parallelzone::HashType;
using parallelzone::HashValue;
using parallelzone::make_hash;
using parallelzone::make_hasher;
} // namespace pluginplay
