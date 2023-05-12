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

#include "pluginplay/printing/detail_/print_inputs.hpp"
#include <catch2/catch.hpp>
#include <utilities/printing/word_wrap_stream.hpp>

using namespace pluginplay::printing::detail_;

TEST_CASE("input_quick_ref_table") {
    pluginplay::type::input_map inputs;

    SECTION("No inputs") {
        auto corr = "+-----+---------+-------------+\n"
                    "| Key | Default | Description |\n"
                    "+-----+---------+-------------+\n";
        REQUIRE(corr == input_quick_ref_table(inputs));
    }
    SECTION("No default or description") {
        auto corr = "+----------+---------+-------------+\n"
                    "| Key      | Default | Description |\n"
                    "+==========+=========+=============+\n"
                    "| Option 1 | N/A     | N/A         |\n"
                    "+----------+---------+-------------+\n";
        inputs["Option 1"];
        REQUIRE(corr == input_quick_ref_table(inputs));
    }
    SECTION("No default") {
        auto corr = "+----------+---------+-------------+\n"
                    "| Key      | Default | Description |\n"
                    "+==========+=========+=============+\n"
                    "| Option 1 | N/A     | Hello world |\n"
                    "+----------+---------+-------------+\n";
        inputs["Option 1"].set_description("Hello world");
        REQUIRE(corr == input_quick_ref_table(inputs));
    }

    SECTION("Fully specified") {
        auto corr = "+----------+---------+-------------+\n"
                    "| Key      | Default | Description |\n"
                    "+==========+=========+=============+\n"
                    "| Option 1 | 3       | Hello world |\n"
                    "+----------+---------+-------------+\n";
        inputs["Option 1"].set_type<int>().set_default(int{3}).set_description(
          "Hello world");
        REQUIRE(corr == input_quick_ref_table(inputs));
    }
    SECTION("Multiple options") {
        auto corr = "+----------+---------+-------------+\n"
                    "| Key      | Default | Description |\n"
                    "+==========+=========+=============+\n"
                    "| Option 1 | 3       | Hello world |\n"
                    "+----------+---------+-------------+\n"
                    "| Option 2 | 3.14    | Bye world   |\n"
                    "+----------+---------+-------------+\n";
        inputs["Option 1"].set_type<int>().set_default(int{3}).set_description(
          "Hello world");
        inputs["Option 2"]
          .set_type<double>()
          .set_default(double{3.14})
          .set_description("Bye world");
        REQUIRE(corr == input_quick_ref_table(inputs));
    }
}
TEST_CASE("input_full_list") {
    std::stringstream ss;
    reSTPrinter p(ss);
    pluginplay::type::input_map inputs;

    SECTION("No inputs") {
        input_full_list(p, inputs);
        REQUIRE(ss.str().empty());
    }
    SECTION("Null input") {
        inputs["Option 1"];
        input_full_list(p, inputs);
        auto corr = "########\n"
                    "Option 1\n"
                    "########\n\n"
                    "- Description : N/A\n"
                    "- Default Value : N/A\n"
                    "- Optional? : False\n"
                    "- Opaque? : True\n"
                    "- Domain Restrictions : N/A\n";

        REQUIRE(ss.str() == corr);
    }
    SECTION("Just a description") {
        inputs["Option 1"].set_description("Hello World");
        input_full_list(p, inputs);
        auto corr = "########\n"
                    "Option 1\n"
                    "########\n\n"
                    "- Description : Hello World\n"
                    "- Default Value : N/A\n"
                    "- Optional? : False\n"
                    "- Opaque? : True\n"
                    "- Domain Restrictions : N/A\n";

        REQUIRE(ss.str() == corr);
    }
    SECTION("Full specification") {
        inputs["Option 1"]
          .set_description("Hello World")
          .set_type<int>()
          .set_default(int{3});
        input_full_list(p, inputs);
        auto corr = std::string("########\n"
                                "Option 1\n"
                                "########\n\n"
                                "- Description : Hello World\n"
                                "- Default Value : 3\n"
                                "- Optional? : False\n"
                                "- Opaque? : True\n"
                                "- Domain Restrictions :\n\n"
                                "  - Type == ") +
                    utilities::printing::Demangler::demangle(typeid(int)) +
                    "\n\n";
        REQUIRE(ss.str() == corr);
    }
    SECTION("Two inputs") {
        auto corr =
          std::string("########\n"
                      "Option 1\n"
                      "########\n\n"
                      "- Description : Hello World\n"
                      "- Default Value : 3\n"
                      "- Optional? : False\n"
                      "- Opaque? : True\n"
                      "- Domain Restrictions :\n\n"
                      "  - Type == ") +
          utilities::printing::Demangler::demangle(typeid(int)) + "\n\n" +
          std::string("########\n"
                      "Option 2\n"
                      "########\n\n"
                      "- Description : By World\n"
                      "- Default Value : 3.14\n"
                      "- Optional? : False\n"
                      "- Opaque? : True\n"
                      "- Domain Restrictions :\n\n"
                      "  - Type == ") +
          utilities::printing::Demangler::demangle(typeid(double)) + "\n\n";

        inputs["Option 1"]
          .set_description("Hello World")
          .set_type<int>()
          .set_default(int{3});
        inputs["Option 2"]
          .set_description("By World")
          .set_type<double>()
          .set_default(double{3.14});
        input_full_list(p, inputs);
        REQUIRE(ss.str() == corr);
    }
}

TEST_CASE("print_inputs") {
    std::stringstream ss;
    reSTPrinter p(ss);
    pluginplay::type::input_map inputs;

    SECTION("No inputs") {
        print_inputs(p, inputs);
        auto corr =
          "#############\n"
          "Module Inputs\n"
          "#############\n"
          "\n"
          "This section details the full list of inputs that the module "
          "accepts. Inputs\n"
          "that are not listed as optional must have their value set "
          "before the module can\n"
          "be run. Inputs can be set manually by calling the module's "
          "``change_input``\n"
          "function or by passing arguments to a property type.\n"
          "\n"
          "***************\n"
          "Quick Reference\n"
          "***************\n"
          "\n"
          "The module defines no inputs.\n"
          "\n"
          "*********************\n"
          "Detailed Descriptions\n"
          "*********************\n"
          "\n"
          "The module defines no inputs.\n";

        REQUIRE(ss.str() == corr);
    }

    SECTION("Inputs") {
        inputs["Option 1"]
          .set_description("Hello World")
          .set_type<int>()
          .set_default(int{3});
        inputs["Option 2"]
          .set_description("By World")
          .set_type<double>()
          .set_default(double{3.14});
        auto corr =
          "#############\n"
          "Module Inputs\n"
          "#############\n"
          "\n"
          "This section details the full list of inputs that the module "
          "accepts. Inputs\n"
          "that are not listed as optional must have their value set before the"
          " module can\n"
          "be run. Inputs can be set manually by calling the module's "
          "``change_input``\n"
          "function or by passing arguments to a property type.\n"
          "\n"
          "***************\n"
          "Quick Reference\n"
          "***************\n"
          "\n"
          "The following table provides a quick summary of the inputs available"
          " to this\n"
          "module. The columns are:\n"
          "\n"
          "- ``Key``: The key used to retrieve the option,\n"
          "- ``Default`` : The initial value, if one exists, and\n"
          "- ``Description`` : A human readable description of what the option"
          " is used for.\n"
          "\n"
          "+----------+---------+-------------+\n"
          "| Key      | Default | Description |\n"
          "+==========+=========+=============+\n"
          "| Option 1 | 3       | Hello World |\n"
          "+----------+---------+-------------+\n"
          "| Option 2 | 3.14    | By World    |\n"
          "+----------+---------+-------------+\n"
          "\n\n"
          "*********************\n"
          "Detailed Descriptions\n"
          "*********************\n"
          "\n"
          "The following subsections provide detailed descriptions of each "
          "input recognized\n"
          "by this module. The subsection headings are the name of the input. "
          "Within each\n"
          "subsection we list:\n"
          "\n"
          "- ``Description`` : A human readable description of what the option "
          "is used for.\n"
          "- ``Default Value`` : The initial value, if one exists.\n"
          "- ``Optional?`` : Whether the value must be set inorder for the "
          "module to run.\n"
          "- ``Opaque?`` : Does the value of the input influence "
          "memoization?.\n"
          "- ``Domain Restrictions`` : Criteria a value must obey to be deemed "
          "valid.\n"
          "\n"
          "Option 1\n"
          "========\n"
          "\n"
          "- Description : Hello World\n"
          "- Default Value : 3\n"
          "- Optional? : False\n"
          "- Opaque? : True\n"
          "- Domain Restrictions :\n"
          "\n"
          "- Type == int\n"
          "\n"
          "Option 2\n"
          "========\n"
          "\n"
          "- Description : By World\n"
          "- Default Value : 3.14\n"
          "- Optional? : False\n"
          "- Opaque? : True\n"
          "- Domain Restrictions :\n"
          "\n"
          "- Type == double\n";
        print_inputs(p, inputs);
    }
}
