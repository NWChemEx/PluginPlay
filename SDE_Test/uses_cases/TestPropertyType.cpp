/*
 * This is a simple example to illustrate some of the basic properties of the
 * SDE. This example defines two property types:
 *
 *  - Area, which is the property type for modules that can compute the area of
 *    a shape.
 *  - Volume, which as you likely guessed, is the property type for modules
 *    that can compute the volume of a solid.
 *
 *  With the definition of the property types we then go on to define some
 *  example modules. First up is the Rectangle class which computes the area of
 *  a rectangle (width times height). One can imagine defining other modules
 *  for other shapes (e.g., a class Triangle that instead returns 1/2 times
 *  width times height). Next is the RectangularPrism module which computes the
 *  volume of a rectangular prism (width times height times length). The last
 *  module computes the volume of any prism (area of base times length) by
 *  calling a module of property type Area.
 *
 *  Some points to note:
 *
 *  - The RectangularPrism module produces the same result as the Prism module
 *    when the area module is set to the Rectangle class. In terms of
 *    computational efficiency the RectangularPrism module is likely to be
 *    faster though. Module system allows you to sacrifice generality for
 *    performance.
 *  - There's a lot of places where a string appears whose content is identical
 *    to the name of a type or a variable.
 *    - For the variables this is an artifact of type-erasure. Namely we need
 *      to establish a mapping between the variables and a string, which is
 *      done most naturally by stringify-ing the variable names. In C++ such
 *      a mapping can only be automated by preprocessor macros.
 *    - For the submodules this repetition allows a module to call multiple
 *      submodules that are all of the same property type.
 *    - For the property types, note that this represents significant code
 *      factorization as compared to going through the expert API since the
 *      same mappings would need to be repeated before every call, but each
 *      property type only needs written once.
 *
 *  Finally, try not to read too much into these examples as they are for show.
 *  There's arguably better ways to implement property types/modules for these
 *  concepts and a lot of the boiler plate could be reduced at the cost of
 *  clarity.
 */

/**************************** Property Types **********************************/

struct Area {
    int run(Module& mod, int width, int height) {
        auto params = mod.params();
        params.change("Width", width);
        params.change("Height", height);
        auto results = mod.run(params);
        return results.value<int>("Area");
    }
};

struct Volume {
    int run(Module& mod, int width, int height, int length){
        auto params = mod.params();
        params.change("Width", width);
        params.change("Height", height);
        params.change("Length", length);
        auto results = mod.run(params);
        return results.value<int>("Volume");
    }
};


/**************************** Modules *****************************************/

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

    //Uses no submodules
    SubmoduleRequestForm submods_() const override { return {}; }

    Parameters params_() const override {
        Parameters p;
        p.insert("Width", Option(int{}, "The width of the shape"));
        p.insert("Height", Option(int{}, "The width of the shape"));
    }
};

class RectangularPrism : public ModuleBase {
private:
    //Define some typedefs to stay under 80 characters
    using const_parameters = const Parameters&;
    using const_submodules = const std::map<std::string, Module>&;

    ComputedProperties run_(const_parameters params, const_submodules&){
        auto w = params.at<int>("Width");
        auto h = params.at<int>("Height");
        auto l = params.at<int>("Length");

        auto v = w * h * l;

        ComputedProperties rvs;
        rvs.emplace("Volume", v);
        return rvs;
    }

    //Uses no submodules
    SubmoduleRequestForm submods_() const override { return {}; }

    Parameters params_() const override {
        Parameters p;
        p.insert("Width", Option(int{}, "The width of the shape"));
        p.insert("Height", Option(int{}, "The width of the shape"));
        p.insert("Length", Option(int{}, "The length of the shape"));
    }
};



class Prism : public ModuleBase {
private:
    //Define some typedefs to stay under 80 characters
    using const_parameters = const Parameters&;
    using const_submodules = const std::map<std::string, Module>&;

    ComputedProperties run_(const_parameters params, const_submodules& submods){
        auto w = params.at<int>("Width");
        auto h = params.at<int>("Height");
        auto l = params.at<int>("Length");

        auto a = submods.at("Area").run_as<Area>(w, h);
        auto v = a * l;

        ComputedProperties rvs;
        rvs.emplace("Volume", v);
        return rvs;
    }

    //Will use one submodule, that submodule should be of property type area
    SubmoduleRequestForm submods_() const override {
        SubmoduleRequestForm form;
        form.push<Area>("Area");
        return form;
    }

    Parameters params_() const override {
        Parameters p;
        p.insert("Width", Option(int{}, "The width of the shape"));
        p.insert("Height", Option(int{}, "The width of the shape"));
        p.insert("Length", Option(int{}, "The length of the shape"));
    }
};

TEST_CASE("Workflow") {
    ModuleManager mm;

}
