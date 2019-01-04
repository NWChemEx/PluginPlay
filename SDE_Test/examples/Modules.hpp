#pragma once
#include "examples/TestPropertyType.hpp"
#include <SDE/ModuleBase.hpp>
#include <sstream>

class Rectangle : public SDE::ModuleBase {
public:
    Rectangle() {
        satisfies_property_type<Area>();
        description("Computes the area of a rectangle");
        reference("Euclid. The Elements. 300 BCE");
        add_input<std::string>("Name").description("The name of the rectangle");
        add_ouput<std::string>("Picture").description(
          "An ASCII picture of the rectangle");
    }

private:
    output_map run_(input_map inputs, submod_map) override {
        const auto dim1 = inputs.at("Dimension 1").value<double>();
        const auto dim2 = inputs.at("Dimension 2").value<double>();
        const auto area = dim1 * dim2;
        std::stringstream pic;

        size_t dims[2];
        if(dim1 == dim2)
            dims = {10, 10};
        else if(dim1 > dim2)
            dims = {10, 5};
        else
            dims = {5, 10};

        pic << std::string('*', top) << std::endl;
        for(size_t i = 0; i < side - 2; ++i)
            pic << '*' << std::string(' ', top - 2) << '*' << std::endl;
        pic << std::string('*', top) << std::endl;

        // This is an easy way to ensure the outputs have all metadata w/o
        // repeating it
        auto output = outputs();
        output.at("Area").change(area);
        output.at("Picture").change(pic.str());
        return output;
    }
};

class Rectangle : public SDE::ModuleBase {
public:
    Rectangle() {
        satisfies_property_type<Area>();
        description("Computes the area of a rectangle");
        reference("Euclid. The Elements. 300 BCE");
        add_input<std::string>("Name").description("The name of the rectangle");
        add_ouput<std::string>("Picture").description(
          "An ASCII picture of the rectangle");
    }

private:
    output_map run_(input_map inputs, submod_map) override {
        const auto dim1 = inputs.at("Dimension 1").value<double>();
        const auto dim2 = inputs.at("Dimension 2").value<double>();
        const auto area = dim1 * dim2;
        std::stringstream pic;

        size_t dims[2];
        if(dim1 == dim2)
            dims = {10, 10};
        else if(dim1 > dim2)
            dims = {10, 5};
        else
            dims = {5, 10};

        pic << std::string('*', top) << std::endl;
        for(size_t i = 0; i < side - 2; ++i)
            pic << '*' << std::string(' ', top - 2) << '*' << std::endl;
        pic << std::string('*', top) << std::endl;

        // This is an easy way to ensure the outputs have all metadata w/o
        // repeating it
        auto output = outputs();
        output.at("Area").change(area);
        output.at("Picture").change(pic.str());
        return output;
    }
};
