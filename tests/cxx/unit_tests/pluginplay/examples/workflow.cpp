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

#include "workflow.hpp"
#include <catch2/catch.hpp>
#include <vector>

TEST_CASE("load_modules") {
    pluginplay::ModuleManager mm;
    load_modules(mm);
    REQUIRE(mm.count("Rectangle") == 1);
    REQUIRE(mm.count("Prism") == 1);
}

TEST_CASE("example_main") { REQUIRE_NOTHROW(example_main()); }
