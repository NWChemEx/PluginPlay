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

#include "pluginplay/detail_/module_pimpl.hpp"
#include "writing_a_module.hpp"
#include <catch2/catch.hpp>

using namespace pluginplay;
using namespace pluginplay::detail_;

TEST_CASE("ModuleBase : Equality") {
    SECTION("Same type") {
        Rectangle r1, r2;
        REQUIRE(r1 == r2);
        REQUIRE(!(r1 != r2));
    }
    SECTION("Different types") {
        Rectangle r;
        Prism p;
        REQUIRE(r != p);
        REQUIRE(!(r == p));
    }
}

TEST_CASE("Rectangle Class : Results") {
    Rectangle r;
    auto results = r.results();
    REQUIRE(results.at("Area").description() == "The area of the shape");
    REQUIRE(results.at("Perimeter").description() ==
            "The perimeter of the shape");
    REQUIRE(results.at("Summary").description() ==
            "A text summary of the results");
}

TEST_CASE("Rectangle Class : Inputs") {
    Rectangle r;
    auto inputs = r.inputs();
    REQUIRE(inputs.at("Dimension 1").description() == "The height of the"
                                                      " rectangle");
    REQUIRE(inputs.at("Dimension 2").description() == "The width of the "
                                                      "rectangle");
    REQUIRE(inputs.at("Name").description() == "The name of the "
                                               "rectangle");
}

TEST_CASE("Rectangle Class : Submods") {
    Rectangle r;
    REQUIRE(r.submods().size() == 0);
}

TEST_CASE("Rectangle Class : Type") {
    Rectangle r;
    REQUIRE(r.type() == typeid(Rectangle));
}

TEST_CASE("Rectangle Class : run") {
    Rectangle r;
    auto in = r.inputs();
    in.at("Dimension 1").change(1.23);
    in.at("Dimension 2").change(4.56);
    in.at("Name").change("Test");
    auto out = r.run(in, r.submods());
    REQUIRE(out.at("Area").value<double>() == Approx(5.6088));
    REQUIRE(out.at("Perimeter").value<double>() == Approx(11.58));
    REQUIRE(out.at("Summary").value<std::string>() ==
            "Test has an area of 5.608800 and a perimeter of 11.580000");
}

TEST_CASE("Prism Class : Inputs") {
    Prism r;
    auto inputs = r.inputs();
    REQUIRE(inputs.at("Dimensions").description() ==
            "1st 2 elements are for the base, last is height");
}

TEST_CASE("Prism Class : Results") {
    Prism r;
    auto results = r.results();
    REQUIRE(results.at("Base area").description() == "The area of the base");
    REQUIRE(results.at("Volume").description() == "The volume of the prism");
}

TEST_CASE("Prism Class : Submods") {
    Prism r;
    auto submods = r.submods();
    REQUIRE(submods.at("area").description() ==
            "Submodule used to compute base's area");
}

TEST_CASE("Prism Class : Type") {
    Prism p;
    REQUIRE(p.type() == typeid(Prism));
}

TEST_CASE("Prism Class : run") {
    Prism r;
    auto in = r.inputs();
    std::vector<double> dims{1.23, 4.56, 7.89};
    in.at("Dimensions").change(dims);
    auto submods = r.submods();

    // Normally the next few lines are done by the ModuleManager
    ModulePIMPL mod_pimpl(std::make_shared<Rectangle>());
    pluginplay::Module mod(std::make_unique<ModulePIMPL>(std::move(mod_pimpl)));
    submods.at("area").change(std::make_shared<pluginplay::Module>(mod));

    auto out            = r.run(in, submods);
    auto [area, volume] = PrismVolume::unwrap_results(out);
    REQUIRE(area == Approx(5.6088));
    REQUIRE(volume == Approx(44.253432));
}

template<size_t N>
struct TemplatedModule : ModuleBase {
    static constexpr size_t size = N;
    TemplatedModule() : ModuleBase(this) { satisfies_property_type<Area>(); }

    type::result_map run_(type::input_map, type::submodule_map) const {
        return type::result_map{};
    }
};

TEST_CASE("TemplatedModule") {
    SECTION("Doesn't need this pointer or template keyword") {
        TemplatedModule<1> test;
        REQUIRE(test.inputs().size() == 2);
    }
}
