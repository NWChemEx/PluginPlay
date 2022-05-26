#pragma once
#include <string>

namespace pluginplay::utility {

/// Type used for universally unique identifiers (UUIDs)
using uuid_type = std::string;

/** @brief Generates a UUID
 *
 *  N.B. This function takes no input. That's because UUIDs are tied to the
 *       time when the UUID was generated and the physical entity generating it.
 *       Not the data the UUID is being generated for.
 *
 *  @return A freshly generated UUID.
 */
uuid_type generate_uuid();

} // namespace pluginplay::utility
