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

#include <catch2/catch.hpp>
#include <iostream>
#include <sstream>

#include "../../../src/pluginplay/printing/helpers.hpp"

using namespace pluginplay::printing;

TEST_CASE("generate_index_string") {
    SECTION("Empty toc") {
        auto corr = ".. toctree::\n"
                    "   :maxdepth: 1\n";

        std::stringstream ss;
        std::vector<std::string> toc;

        generate_index_file(ss, toc);

        REQUIRE(ss.str() == corr);
    }

    SECTION("Full toc") {
        auto corr = ".. toctree::\n"
                    "   :maxdepth: 1\n"
                    "\n"
                    "   Coulomb's_Law\n"
                    "   Coulomb's_Law_with_screening\n"
                    "   Force\n"
                    "   Single-precision_Coulomb's_law";

        std::stringstream ss;
        std::vector<std::string> toc = {"Coulomb's_Law",
                                        "Coulomb's_Law_with_screening", "Force",
                                        "Single-precision_Coulomb's_law"};

        generate_index_file(ss, toc);

        REQUIRE(ss.str() == corr);
    }
}

TEST_CASE("replace_invalid_chars") {
    SECTION("Empty string") {
        auto corr = "";

        std::string clean_str = replace_invalid_chars("", "<>:\"|?*\\/ ", '_');

        REQUIRE(clean_str == corr);
    }

    SECTION("Empty invalid chars") {
        auto corr = "test";

        std::string clean_str = replace_invalid_chars("test", "", '_');

        REQUIRE(clean_str == corr);
    }

    SECTION("Replace all") {
        auto corr = "__________";

        std::string clean_str =
          replace_invalid_chars("<>:\"|?*\\/ ", "<>:\"|?*\\/ ", '_');

        REQUIRE(clean_str == corr);
    }
}
