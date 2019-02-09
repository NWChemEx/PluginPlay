#pragma once
#include <string>

namespace bphash {
class Hasher;
}

namespace SDE {
namespace detail_ {
class SDEAny;
} // namespace detail_

/** @brief  Holds typedefs of types commonly used throughout the SDE.
 *
 *  Although C++11's auto helps a lot, in general it is best if you declare
 *  types once and use typedefs throughout a library. In the event that you
 *  need to change a type, this practice makes it so that you only have to
 *  change the type in one place. Placing the typedefs in a nested namespace
 *  avoids polluting the global namespace with our typedefs if the user decides
 *  to do `using namespace std;` or something similar.
 */
namespace type {

/// The type of our type-erased holder class
using any = detail_::SDEAny;

/// The type of metadata holding a description
using description = std::string;

/// Type of the object that hashes objects.
using hasher = bphash::Hasher;

/// Key type for any SDE class behaving like an associative container
using key = std::string;

/// Type of a natural number, including zero
using size = std::size_t;

} // namespace type
} // namespace SDE
