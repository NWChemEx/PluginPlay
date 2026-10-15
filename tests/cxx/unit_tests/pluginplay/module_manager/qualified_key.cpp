/*
 * Copyright 2026 NWChemEx-Project
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

#include "../catch.hpp"
#include <pluginplay/module_manager/qualified_key.hpp>

using pluginplay::QualifiedKey;

TEST_CASE("QualifiedKey") {
    QualifiedKey key_only("Overlap");
    QualifiedKey qualified("integrals", "Overlap");
    QualifiedKey own("", "Overlap");

    SECTION("Key-only") {
        REQUIRE_FALSE(key_only.is_qualified());
        REQUIRE(key_only.plugin().empty());
        REQUIRE(key_only.key() == "Overlap");
        REQUIRE(key_only.to_string() == "Overlap");

        // Implicit from std::string too
        QualifiedKey from_string = std::string("Overlap");
        REQUIRE(from_string == key_only);
    }

    SECTION("Qualified") {
        REQUIRE(qualified.is_qualified());
        REQUIRE(qualified.plugin() == "integrals");
        REQUIRE(qualified.key() == "Overlap");
        REQUIRE(qualified.to_string() == "integrals::Overlap");

        // The empty plugin name still makes a qualified key
        REQUIRE(own.is_qualified());
        REQUIRE(own.plugin().empty());
    }

    SECTION("Comparisons") {
        // Module keys are case-insensitive
        REQUIRE(qualified == QualifiedKey("integrals", "OVERLAP"));
        REQUIRE(key_only == QualifiedKey("overlap"));

        // Plugin names are case-insensitive too
        REQUIRE(qualified == QualifiedKey("Integrals", "Overlap"));
        REQUIRE(qualified != QualifiedKey("nux", "Overlap"));

        REQUIRE(qualified != key_only);
        REQUIRE(own != key_only);
        REQUIRE(qualified != QualifiedKey("integrals", "Overlaps"));
    }
}
