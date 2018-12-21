#include <SDE/attic/ModuleBase.hpp> //Needed for defining the modules
using namespace SDE; //Shortens the example, in practice avoid using in headers

/*
 * This example demonstrates how one creates a property type and how one makes
 * a module. These two concepts are intimately related in that a property type
 * defines a computable property (as well as the runtime API for computing that
 * property) and the module provides a realization of an algorithm for computing
 * that property. This example focuses on writing modules that can compute the
 * area and volume of polygons and solids respectively. Try not to read too much
 * into these examples as they are intended to showcase the property type/module
 * concept more than they are intended to be production quality means of
 * computing areas and volumes.
 *
 * We start by defining the property types. Generally speaking, this would only
 * be done once and would be used across an entire project. This ensures that
 * everyone uses the same API for computing the same properties (here the
 * properties are "Area" and "Volume"). Each property type tells the SDE what
 * inputs and what outputs are part of its API. The order they are declared in
 * defines the map between the compile time argument order and the runtime key,
 * *e.g.*, the definition:
 *
 *     add_input<int>("Param1");
 *     add_input<double>("Param2");
 *     add_return<double>("return");
 *
 * creates an API `double run(int, double)` and:
 *
 *     add_input<double>("Param1");
 *     add_input<int>("Param2");
 *     add_return<double>("return");
 *
 * maps to an API `double run(double, int)`.
 *
 * The functions `add_input` and `add_return` are defined by the PropertyType
 * base class that all property types must inherit from.
 */

struct Area : PropertyType {
    Area() {
        add_input<int>("Width").description("The width of the polygon");
        add_input<int>("Height").description("The height of the polygon");
        add_result<int>("Area").description("The area of the polygon");
    }
};

struct Volume : PropertyType {
    Volume() {
        add_input<int>("Width").description("The width of the solid");
        add_input<int>("Height").description("The height of the solid");
        add_input<int>("Length").description("The length of the solid");
        add_return<int>("Volume").description("The volume of the solid");
    }
};

/* With the property types defined we turn our attention to writing the actual
 * modules.
 *
 * The first module we write is for computing the area of a rectangle. The
 * actual implementation of the algorithm occurs within the `run_` function and
 * is relatively straightforward. The remainder of the module declaration occurs
 * in the constructor, here we state that the module satisfies the property type
 * `Area`. The set of input parameters is minimally the union of the input
 * parameters of all satisfied property types. The returns are determined in a
 * similar manner. As shown in the constructor your module is free to define
 * additional parameters (here we define a string parameter associated with the
 * key "Name").
 */

class Rectangle : public ModuleBase {
private:
    //Define some typedefs to stay under 80 characters
    using const_parameters = const Parameters&;
    using const_submodules = const std::map<std::string, Module>&;

    ComputedProperties run_(const_parameters params, const_submodules&){
        auto w = params.at<int>("Width");
        auto h = params.at<int>("Height");
        auto a = w * h;

        ComputedProperties rvs;
        rvs.emplace("Area", a);
        return rvs;
    }

    Rectangle() {
        satisfies_property_type<Area>();
        add_input<std::string>("Name");
    }
};

class RectangularPrism : public ModuleBase {
private:
    //Define some typedefs to stay under 80 characters
    using const_parameters = const Parameters&;
    using const_submodules = const std::map<std::string, Module>&;

    ComputedProperties run_(const_parameters params, const_submodules&){
        auto [w, h, l] = params.at<int, int, int>("Width", "Height", "Length");
        auto v = w * h * l;

        ComputedProperties rvs;
        rvs.emplace("Volume", v);
        return rvs;
    }

    RectangularPrism() {
        satisfies_property_type<Volume>();
    }
};



class Prism : public ModuleBase {
private:
    //Define some typedefs to stay under 80 characters
    using const_parameters = const Parameters&;
    using const_submodules = const std::map<std::string, Module>&;

    ComputedProperties run_(const_parameters params, const_submodules& submods){
        auto [w, h, l] = params.at<int, int, int>("Width", "Height", "Length");
        auto a = submods.at("Area").run_as<Area>(w, h);
        auto v = a * l;

        ComputedProperties rvs;
        rvs.emplace("Volume", v);
        return rvs;
    }

    Prism() {
        satisfies_property_type<Volume>();
        add_submodule<Area>("Area");
    }
};
