#pragma once
#include "examples/TestPropertyType.hpp"
#include <SDE/ModuleBase.hpp>
#include <sstream>

class Rectangle : public SDE::ModuleBase {
public:
    Rectangle() {
        // Piggy-backs off of the Area property type's API
        satisfies_property_type<Area>();

        // Add some metadata to a module
        description("Computes the area of a rectangle");
        reference("Euclid. The Elements. 300 BCE");

        // Add additional elements to the module's API
        add_input<std::string>("Name").description("The name of the rectangle");
        add_ouput<std::string>("Picture").description(
          "An ASCII picture of the rectangle");

        // Modify existing API elements
        inputs().at("Dimension 1").desc = "The height of the rectangle";
        inputs().at("Dimension 2").desc = "The width of the rectangle";
    }

private:
    output_map run_(input_map inputs, submod_map) const override {
        // Read in the input
        //{
        const auto dim1 = inputs.at("Dimension 1").value<double>();
        const auto dim2 = inputs.at("Dimension 2").value<double>();
        //}

        // Compute the area
        const auto area = dim1 * dim2;

        // Compute the ASCII picture we agreed to write
        //{
        std::stringstream pic;

        size_t dims[2];
        if(dim1 == dim2)
            dims = {10, 10};
        else if(dim1 > dim2)
            dims = { 10, 5 } else dims = {5, 10};

        // Note: std::string(char c, int N) sets string to N copies of the char
        // c
        pic << std::string('*', dims[1]) << std::endl; // Top of rectangle
        for(size_t i = 0; i < dims[0] - 2; ++i)
            pic << '*' << std::string(' ', dims[1] - 2) << '*' << std::endl;
        pic << std::string('*', dims[1]) << std::endl; // bottom of rectangle
        //}

        // Set the outputs
        //{
        auto output = outputs(); // Ensures our outputs have all metadata
        output.at("Area").change(area);
        output.at("Picture").change(pic.str());
        //}

        // Return our computed values
        return output;
    }
};

class Prism : public SDE::ModuleBase {
public:
    Prism() {
        // Same roles as in Rectangle
        //{
        satisfies_property_type<PrismVolume>();
        description("Computes the area of a prism");
        reference("Euclid. The Elements. 300 BCE");
        inputs().at("Dimensions").desc = "First 2 dimensions are for the base"
          //}

          // Registers that this module will use a submodule that satisfies the
          // property type of Area
          add_submodule<Area>("area", "Submodule used to compute base's area");
    }

private:
    output_map run_(input_map inputs, submod_map submods) const override {
        const auto& dims =
          inputs.at("Dimensions").value<const std::vector<double>&>();
        auto area   = submods.at("area").run_as<Area>(dims[0], dims[1]);
        auto volume = area * dims[2];

        auto output = outputs();
        output.at("Base area").change(area);
        output.at("Volume").change(volume);
        return output;
    }
};
