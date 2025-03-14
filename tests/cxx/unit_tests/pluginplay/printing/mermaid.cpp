/*
 * Copyright 2024 NWChemEx-Project
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
#include "../test_common.hpp"
#include "unit_testing_pts.hpp"
#include <catch2/catch_test_macros.hpp>
#include <pluginplay/module/macros.hpp>
#include <pluginplay/module_manager/module_manager.hpp>
#include <pluginplay/printing/mermaid.hpp>
#include <sstream>
#include <string>

DECLARE_MODULE(OneModule);
inline MODULE_CTOR(OneModule) { satisfies_property_type<testing::NullPT>(); }
inline MODULE_RUN(OneModule) { return results(); }

DECLARE_MODULE(TwoModule);
inline MODULE_CTOR(TwoModule) { satisfies_property_type<testing::NullPT>(); }
inline MODULE_RUN(TwoModule) { return results(); }

DECLARE_MODULE(ThreeModule);
inline MODULE_CTOR(ThreeModule) { satisfies_property_type<testing::NullPT>(); }
inline MODULE_RUN(ThreeModule) { return results(); }

DECLARE_MODULE(OneModuleSubTwo);
inline MODULE_CTOR(OneModuleSubTwo) {
    satisfies_property_type<testing::NullPT>();
    add_submodule<testing::NullPT>("Submodule");
}
inline MODULE_RUN(OneModuleSubTwo) { return results(); }

DECLARE_MODULE(TwoModuleSubThree);
inline MODULE_CTOR(TwoModuleSubThree) {
    satisfies_property_type<testing::NullPT>();
    add_submodule<testing::NullPT>("Submodule");
}
inline MODULE_RUN(TwoModuleSubThree) { return results(); }

DECLARE_MODULE(OneModuleSubTwoThree);
inline MODULE_CTOR(OneModuleSubTwoThree) {
    satisfies_property_type<testing::NullPT>();
    add_submodule<testing::NullPT>("Submodule 1");
    add_submodule<testing::NullPT>("Submodule 2");
}
inline MODULE_RUN(OneModuleSubTwoThree) { return results(); }

TEST_CASE("Mermaid Graph") {
    SECTION("No Modules") {
        pluginplay::ModuleManager no_mods;
        auto hello = pluginplay::printing::create_mermaid_graph(no_mods);
        REQUIRE(hello.str() == "No modules are loaded, load some modules!");
    }

    SECTION("One Module") {
        pluginplay::ModuleManager one_mod;
        one_mod.add_module<OneModule>("One Module");
        auto onemod     = pluginplay::printing::create_mermaid_graph(one_mod);
        std::string val = "\n```mermaid\nflowchart LR\n\tAAA[One Module]\n```";

        REQUIRE(val == onemod.str());
    }

    SECTION("2 Modules, Not Related") {
        std::stringstream ss;
        pluginplay::ModuleManager two_mods_unrelated;
        two_mods_unrelated.add_module<OneModule>("One Module");
        two_mods_unrelated.add_module<TwoModule>("Two Module");
        auto twomod =
          pluginplay::printing::create_mermaid_graph(two_mods_unrelated);
        ss << "\n```mermaid\nflowchart LR\n\tAAA[One Module]\n```";
        ss << "\n```mermaid\nflowchart LR\n\tAAB[Two Module]\n```";

        REQUIRE(ss.str() == twomod.str());
    }

    SECTION("2 Modules, Related") {
        std::stringstream ss2;
        pluginplay::ModuleManager two_mods_related;
        two_mods_related.add_module<OneModuleSubTwo>("One Module Submod Two");
        two_mods_related.add_module<TwoModule>("Two Module");
        two_mods_related.change_submod("One Module Submod Two", "Submodule",
                                       "Two Module");
        auto twomodrelated =
          pluginplay::printing::create_mermaid_graph(two_mods_related);
        ss2 << "\n```mermaid\nflowchart LR\n\tAAA[One Module Submod Two]\n";
        ss2 << "\tAAA-->|Submodule| AAB[Two Module]\n```";
        ss2 << "\n```mermaid\nflowchart LR\n\tAAC[Two Module]\n```";
        REQUIRE(ss2.str() == twomodrelated.str());
    }

    SECTION("3 Modules, Nested") {
        std::stringstream ss3;
        pluginplay::ModuleManager three_mods_recursive;
        three_mods_recursive.add_module<OneModuleSubTwo>(
          "One Module Submod Two");
        three_mods_recursive.add_module<TwoModuleSubThree>(
          "Two Module Submod Three");
        three_mods_recursive.add_module<ThreeModule>("Three Module");
        three_mods_recursive.change_submod("One Module Submod Two", "Submodule",
                                           "Two Module Submod Three");
        three_mods_recursive.change_submod("Two Module Submod Three",
                                           "Submodule", "Three Module");
        auto threemodrecursive =
          pluginplay::printing::create_mermaid_graph(three_mods_recursive);
        ss3 << "\n```mermaid\n";
        ss3 << "flowchart LR\n";
        ss3 << "\tAAA[One Module Submod Two]\n";
        ss3 << "\tAAA-->|Submodule| AAB[Two Module Submod Three]\n";
        ss3 << "\tAAB-->|Submodule| AAC[Three Module]\n";
        ss3 << "```";
        ss3 << "\n```mermaid\n";
        ss3 << "flowchart LR\n";
        ss3 << "\tAAD[Three Module]\n";
        ss3 << "```";
        ss3 << "\n```mermaid\n";
        ss3 << "flowchart LR\n";
        ss3 << "\tAAE[Two Module Submod Three]\n";
        ss3 << "\tAAE-->|Submodule| AAF[Three Module]\n";
        ss3 << "```";

        REQUIRE(ss3.str() == threemodrecursive.str());
    }

    SECTION("3 Modules, Branched") {
        std::stringstream ss4;
        pluginplay::ModuleManager three_mods_branched;
        three_mods_branched.add_module<OneModuleSubTwoThree>(
          "One Module Submod Two Three");
        three_mods_branched.add_module<OneModule>("One Module");
        three_mods_branched.add_module<TwoModule>("Two Module");
        three_mods_branched.change_submod("One Module Submod Two Three",
                                          "Submodule 1", "One Module");
        three_mods_branched.change_submod("One Module Submod Two Three",
                                          "Submodule 2", "Two Module");
        auto threemodsbranched =
          pluginplay::printing::create_mermaid_graph(three_mods_branched);

        ss4 << "\n```mermaid\n";
        ss4 << "flowchart LR\n";
        ss4 << "\tAAA[One Module]\n";
        ss4 << "```";
        ss4 << "\n```mermaid\n";
        ss4 << "flowchart LR\n";
        ss4 << "\tAAB[One Module Submod Two Three]\n";
        ss4 << "\tAAB-->|Submodule 1| AAC[One Module]\n";
        ss4 << "\tAAB-->|Submodule 2| AAD[Two Module]\n";
        ss4 << "```";
        ss4 << "\n```mermaid\n";
        ss4 << "flowchart LR\n";
        ss4 << "\tAAE[Two Module]\n";
        ss4 << "```";

        REQUIRE(ss4.str() == threemodsbranched.str());
    }
}
