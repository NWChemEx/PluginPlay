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

#include "../../test_common.hpp"
#include "pluginplay/printing/detail_/print_submodules.hpp"
#include <catch2/catch.hpp>
#include <utilities/printing/word_wrap_stream.hpp>

using namespace pluginplay::printing::detail_;

TEST_CASE("submod_table") {
    pluginplay::type::submodule_map submods;

    SECTION("No submods") {
        auto corr = "+-----+---------------+-------------+\n"
                    "| Key | Property Type | Description |\n"
                    "+-----+---------------+-------------+\n";
        REQUIRE(submod_table(submods) == corr);
    }
    SECTION("Empty Submod") {
        submods["Submod1"];
        auto corr = "+---------+---------------+-------------+\n"
                    "| Key     | Property Type | Description |\n"
                    "+=========+===============+=============+\n"
                    "| Submod1 | N/A           | N/A         |\n"
                    "+---------+---------------+-------------+\n";
        REQUIRE(submod_table(submods) == corr);
    }
    SECTION("Has description") {
        submods["Submod1"].set_description("Hello World");
        auto corr = "+---------+---------------+-------------+\n"
                    "| Key     | Property Type | Description |\n"
                    "+=========+===============+=============+\n"
                    "| Submod1 | N/A           | Hello World |\n"
                    "+---------+---------------+-------------+\n";
        REQUIRE(submod_table(submods) == corr);
    }
    SECTION("Has property type") {
        submods["Submod1"].set_type<testing::NullPT>();
        auto corr = "+---------+-----------------+-------------+\n"
                    "| Key     | Property Type   | Description |\n"
                    "+=========+=================+=============+\n"
                    "| Submod1 | testing::NullPT | N/A         |\n"
                    "+---------+-----------------+-------------+\n";
        REQUIRE(submod_table(submods) == corr);
    }
    SECTION("The whole shebang") {
        submods["Submod1"].set_type<testing::NullPT>().set_description(
          "Hello World");
        auto corr = "+---------+-----------------+-------------+\n"
                    "| Key     | Property Type   | Description |\n"
                    "+=========+=================+=============+\n"
                    "| Submod1 | testing::NullPT | Hello World |\n"
                    "+---------+-----------------+-------------+\n";
        REQUIRE(submod_table(submods) == corr);
    }
}

TEST_CASE("print_submods") {
    pluginplay::type::submodule_map submods;
    std::stringstream ss;
    reSTPrinter p(ss);
    SECTION("No submods") {
        auto corr = "##########\n"
                    "Submodules\n"
                    "##########\n"
                    "\n"
                    "The module defines no submodules.\n\n";
        print_submods(p, submods);
        REQUIRE(ss.str() == corr);
    }
    SECTION("Submodules") {
        submods["Submod1"].set_type<testing::NullPT>().set_description(
          "Hello World");
        submods["Sumbod2"].set_type<testing::OneIn>().set_description(
          "Bye World");
        print_submods(p, submods);
        auto corr = "##########\n"
                    "Submodules\n"
                    "##########\n"
                    "\n"
                    "This section details the full list of submodules that the "
                    "module uses. For each\n"
                    "submodule we have listed:\n"
                    "\n"
                    "- Key : The key used to refer to this particular "
                    "callback.\n"
                    "- Property Type : The property type that the submodule "
                    "must satisfy.\n"
                    "- Description : How the module will use the submodule.\n"
                    "\n"
                    "+---------+-----------------+-------------+\n"
                    "| Key     | Property Type   | Description |\n"
                    "+=========+=================+=============+\n"
                    "| Submod1 | testing::NullPT | Hello World |\n"
                    "+---------+-----------------+-------------+\n"
                    "| Sumbod2 | testing::OneIn  | Bye World   |\n"
                    "+---------+-----------------+-------------+\n";
        REQUIRE(ss.str() == corr);
    }
}
