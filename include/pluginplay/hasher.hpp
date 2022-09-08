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
