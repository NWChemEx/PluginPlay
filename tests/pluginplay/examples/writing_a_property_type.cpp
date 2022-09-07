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

#include "writing_a_property_type.hpp"
#include <catch2/catch.hpp>

/* This test focuses on the property_type class. The property_type class can not
 * be directly tested as it is an abstract class relying on compile-time
 * polymorphism. For this reason we test two example property types: "Area" and
 * "Volume". The code for these classes is contained within the
 * "examples/TestPropertyType.hpp" header file. These property types will be
 * used later for other examples tests.
 *
 */

TEST_CASE("Area Property Type") {
    Area pt;
    SECTION("Inputs") {
        auto inputs = pt.inputs();
        REQUIRE(inputs.size() == 2);
        REQUIRE(inputs["Dimension 1"].description() ==
                "The length of the 1st dimension");
        REQUIRE(inputs["Dimension 2"].description() ==
                "The length of the 2nd dimension");
    }

    SECTION("Wrap Inputs") {
        // Uncomment to check static assertion
        // pt.wrap_inputs(std::string{"Hi"}, double{3.14});
        auto inputs = pt.wrap_inputs(pt.inputs(), double{1.23}, double{4.56});
        SECTION("Manually unwrap") {
            REQUIRE(inputs.size() == 2);
            REQUIRE(inputs["Dimension 1"].value<double>() == 1.23);
            REQUIRE(inputs["Dimension 2"].value<double>() == 4.56);
        }
        SECTION("Auto unwrap") {
            auto [dim1, dim2] = Area::unwrap_inputs(inputs);
            REQUIRE(dim1 == 1.23);
            REQUIRE(dim2 == 4.56);
        }
    }

    SECTION("Results") {
        auto results = pt.results();
        REQUIRE(results.size() == 1);
        REQUIRE(results["Area"].description() == "The area of the shape");
    }
    SECTION("Wrap Results") {
        auto results = Area::wrap_results(pt.results(), double{1.23});
        SECTION("Manually unwrap") {
            REQUIRE(results.at("Area").value<double>() == 1.23);
        }

        SECTION("Unwrap Auto Wrapped Results") {
            auto [area] = Area::unwrap_results(results);
            REQUIRE(area == 1.23);
        }
    }
}

TEST_CASE("Perimeter Property Type") {
    Perimeter pt;
    SECTION("Inputs") {
        auto inputs = pt.inputs();
        REQUIRE(inputs.size() == 2);
        REQUIRE(inputs["Dimension 1"].description() ==
                "The length of the 1st dimension");
        REQUIRE(inputs["Dimension 2"].description() ==
                "The length of the 2nd dimension");
    }

    SECTION("Wrap Inputs") {
        // Uncomment to check static assertion
        // pt.wrap_inputs(std::string{"Hi"}, double{3.14});
        auto inputs = pt.wrap_inputs(pt.inputs(), double{1.23}, double{4.56});
        SECTION("Manually unwrap") {
            REQUIRE(inputs.size() == 2);
            REQUIRE(inputs["Dimension 1"].value<double>() == 1.23);
            REQUIRE(inputs["Dimension 2"].value<double>() == 4.56);
        }
        SECTION("Auto unwrap") {
            auto [dim1, dim2] = Perimeter::unwrap_inputs(inputs);
            REQUIRE(dim1 == 1.23);
            REQUIRE(dim2 == 4.56);
        }
    }

    SECTION("Results") {
        auto results = pt.results();
        REQUIRE(results.size() == 1);
        REQUIRE(results["Perimeter"].description() ==
                "The perimeter of the shape");
    }

    SECTION("Wrap Results") {
        auto results = Perimeter::wrap_results(pt.results(), double{1.23});

        SECTION("Manually unwrap") {
            REQUIRE(results.at("Perimeter").value<double>() == 1.23);
        }

        SECTION("Unwrap Auto Wrapped Results") {
            auto [area] = Perimeter::unwrap_results(results);
            REQUIRE(area == 1.23);
        }
    }
}

TEST_CASE("PrismVolume Property Type") {
    PrismVolume pt;
    SECTION("Inputs") {
        auto inputs = pt.inputs();
        REQUIRE(inputs.size() == 1);
        REQUIRE(inputs["Dimensions"].description() ==
                "The length of each dimension");
    }

    SECTION("Wrap Inputs") {
        using v_double = std::vector<double>;
        v_double dims{1.23, 4.56, 7.89};
        auto inputs = pt.inputs();
        pt.wrap_inputs(inputs, dims);
        REQUIRE(inputs.size() == 1);
        SECTION("Manually unwrap") {
            const auto& pdims = inputs["Dimensions"].value<const v_double&>();
            REQUIRE(&pdims == &dims);
        }
        SECTION("Auto unwrap Inputs") {
            const auto& [pdims] = PrismVolume::unwrap_inputs(inputs);
            REQUIRE(&pdims == &dims);
        }
    }

    SECTION("Results") {
        auto results = pt.results();
        REQUIRE(results.size() == 2);
        REQUIRE(results["Base area"].description() == "The area of the base");
        REQUIRE(results["Volume"].description() == "The volume of the prism");
    }

    SECTION("Wrap Results") {
        double v1    = 1.23;
        double v2    = 4.56;
        auto results = pt.results();
        results      = PrismVolume::wrap_results(results, v1, v2);
        SECTION("Manually unwrap") {
            REQUIRE(results.at("Base area").value<double>() == v1);
            REQUIRE(results.at("Volume").value<double>() == v2);
        }
        SECTION("Auto unwrap Results") {
            auto [area, volume] = pt.unwrap_results(results);
            REQUIRE(area == 1.23);
            REQUIRE(volume == 4.56);
        }
    }
}

template<size_t N>
struct TemplatedPT : pluginplay::PropertyType<TemplatedPT<N>> {
    static constexpr size_t size = N;
    auto inputs_() {
        return pluginplay::declare_input().add_field<double>("Test param");
    }
};

TEST_CASE("Templated Property Type") {
    SECTION("Works without the this pointer and template keyword") {
        TemplatedPT<1> test;
        REQUIRE(test.inputs_().size() == 1);
    }
}
