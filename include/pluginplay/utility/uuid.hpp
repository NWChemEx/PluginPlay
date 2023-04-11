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
