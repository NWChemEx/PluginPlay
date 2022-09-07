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
#include "pluginplay/printing/detail_/print_results.hpp"
#include <catch2/catch.hpp>
#include <utilities/printing/word_wrap_stream.hpp>

using namespace pluginplay::printing::detail_;

TEST_CASE("result_table") {
    pluginplay::type::result_map results;

    SECTION("No results") {
        auto corr = "+-----+------+-------------+\n"
                    "| Key | Type | Description |\n"
                    "+-----+------+-------------+\n";
        REQUIRE(result_table(results) == corr);
    }
    SECTION("Empty result") {
        results["Result 1"];
        auto corr = "+----------+------+-------------+\n"
                    "| Key      | Type | Description |\n"
                    "+==========+======+=============+\n"
                    "| Result 1 | N/A  | N/A         |\n"
                    "+----------+------+-------------+\n";
        REQUIRE(result_table(results) == corr);
    }
    SECTION("Has description") {
        results["Result 1"].set_description("Hello World");
        auto corr = "+----------+------+-------------+\n"
                    "| Key      | Type | Description |\n"
                    "+==========+======+=============+\n"
                    "| Result 1 | N/A  | Hello World |\n"
                    "+----------+------+-------------+\n";
        REQUIRE(result_table(results) == corr);
    }
    SECTION("Has type") {
        results["Result 1"].set_type<double>();
        auto corr = "+----------+--------+-------------+\n"
                    "| Key      | Type   | Description |\n"
                    "+==========+========+=============+\n"
                    "| Result 1 | double | N/A         |\n"
                    "+----------+--------+-------------+\n";
        REQUIRE(result_table(results) == corr);
    }
    SECTION("The whole shebang") {
        results["Result 1"].set_type<double>().set_description("Hello World");
        auto corr = "+----------+--------+-------------+\n"
                    "| Key      | Type   | Description |\n"
                    "+==========+========+=============+\n"
                    "| Result 1 | double | Hello World |\n"
                    "+----------+--------+-------------+\n";
        REQUIRE(result_table(results) == corr);
    }
}

TEST_CASE("print_results") {
    pluginplay::type::result_map results;
    std::stringstream ss;
    reSTPrinter p(ss);
    SECTION("No results") {
        auto corr = "##############\n"
                    "Module Results\n"
                    "##############\n"
                    "\n"
                    "The module defines no results.\n\n";
        print_results(p, results);
        REQUIRE(ss.str() == corr);
    }
    SECTION("results") {
        results["Results 1"].set_type<double>().set_description("Hello World");
        results["Results 2"].set_type<double>().set_description("Bye World");
        print_results(p, results);
        auto corr = "##############\n"
                    "Module Results\n"
                    "##############\n"
                    "\n"
                    "This section tabulates the full list of results that the "
                    "module returns. The\n"
                    "columns respectively are:\n"
                    "\n"
                    "- Key: What the result is called\n"
                    "- Type: The C++ type of the result\n"
                    "- Description: What the result is/how it was computed.\n"
                    "\n"
                    ".. note::\n"
                    "\n"
                    "   A given property type will only return a subset of the "
                    "available results. \n"
                    "   Additional results can be accessed by using other "
                    "property types or by using\n"
                    "   the Module class's advanced API.\n"
                    "\n"
                    "+-----------+--------+-------------+\n"
                    "| Key       | Type   | Description |\n"
                    "+===========+========+=============+\n"
                    "| Results 1 | double | Hello World |\n"
                    "+-----------+--------+-------------+\n"
                    "| Results 2 | double | Bye World   |\n"
                    "+-----------+--------+-------------+\n"
                    "\n\n";
        REQUIRE(ss.str() == corr);
    }
}
