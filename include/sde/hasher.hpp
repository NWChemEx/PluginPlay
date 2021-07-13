#include <parallelzone/hasher.hpp>
/** @file hasher.hpp
 *
 * This header injects hashing utilities into sde namespace.
 *
 */

namespace sde {
using pz::hash_objects;
using pz::hash_to_string;
using pz::Hasher;
using pz::HashType;
using pz::HashValue;
using pz::make_hash;
using pz::make_hasher;
} // namespace sde
