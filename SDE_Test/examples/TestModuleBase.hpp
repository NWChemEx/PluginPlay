#pragma once
#include "examples/TestPropertyType.hpp"
#include <SDE/ModuleBase.hpp>
#include <sstream>

/* The Rectangle module implements the algorithm for computing the area of a
 * rectangle. The actual class showcases:
 *
 *  - How to define the API of a module by using a property type
 *  - How to set the metadata (e.g., description, references) of a module
 *  - How to add inputs/results beyond the property type's API
 *  - How to modify metadata of the property type's API to make them more
 *    applicable to the current module.
 *  - How to implement the module's algorithm
 */
class Rectangle : public SDE::ModuleBaseHelper<Rectangle> {
public:
    Rectangle() {
        /* Most modules will be designed with a property type in mind. For
         * example this module was designed with the explicit intent of making
         * a module that computes Areas. In order to satisfy a property type
         * all of the input fields and output fields of that property type must
         * be included in the module's sets of input and output fields. For the
         * Area property type that means we'd minimally need to write something
         * like:
         *
         * add_input<double>("Dimension 1").set_description(...);
         * add_input<double>("Dimension 2").set_description(...);
         * add_output<double>("Area").set_description(...);
         *
         * This boilerplate code that adds no new information, hence this can be
         * automated for you by using the satisfies_property_type member
         * function. The satisfies_property_type function automatically adds the
         * specified property type's inputs and results to this module's inputs
         * and results.
         *
         * As a note, if a module satisfies multiple property types and any of
         * those property types include the same input/output parameter only the
         * metadata associated with the last property type added will be stored.
         * In practice this is rarely an issue since if two property types have
         * a parameter with the same name they usually use that parameter in
         * identical manners.
         */
        satisfies_property_type<Area>();

        /* In addition to the metadata associated with each input and output
         * there is also metadata associated with the module itself. Here we
         * set the module's documentation description and add citations for any
         * relevant literature.
         */
        description("Computes the area of a rectangle");
        citation("Euclid. The Elements. 300 BCE");

        /* The input/results of a module are not limited to those specified by
         * the property type. You are free to add additional inputs and results.
         * However, users using your module through a property type not
         * defining these additional inputs/results will not be able to
         * directly access these fields. Nevertheless the additional
         * inputs/results can always be accessed via the expert API and may be
         * accessible from other property type APIs. This means that
         * particularly for inputs it is best if these additional inputs are
         * algorithmic parameters with default values (additional results can
         * always be ignored no problem and thanks to memoization need not be
         * recomputed if they are later needed). Inevitably, there will be times
         * when no reasonable default exists; in these cases users can set the
         * parameters directly on the module (typically via the ModuleManager)
         * like:
         *
         * ModuleManager mm;//Assume this got filled somehow
         * mm.at("Key to module") //get module
         *   .inputs() //get inputs of module
         *   .at("Input not part of property type") //get additional input
         *   .change(value_to_set_it_to); //change inputs value
         *
         * //Sometime later
         * mm.at("Key to module") //get module
         *   .run_as<PropType>(args...); //run w/o specifying additional input
         *
         * In this code example the additional input is set directly on the
         * module that will be used. This value is tied to the module with the
         * module key "Key to module" meaning that other instances of the module
         * will not have this value specified. Later when we use the module via
         * the property type that does not recognize the additional field we
         * only specify the inputs that the property type does recognize and the
         * value for the unrecognized field is taken from the module's state.
         *
         * Anyways, we add an additional input that names our rectangle (perhaps
         * for printing) and an additional output which is a ASCII picture of
         * of our rectangle (yes, I'm grasping at straws for what else a
         * rectangle class needs...).
         */
        add_input<std::string>("Name")
          .set_description("The name of the rectangle")
          .set_default("");
        add_output<std::string>("Picture").set_description(
          "An ASCII picture of the rectangle");

        /* Particularly when a field is set from a property type the metadata
         * may be quite generic. In these cases the module designer is able to
         * update the metadata by directly accessing it. To illustrate this
         * point we update the descriptions for the two input parameters to
         * reflect that they will be used as the height and width of the
         * rectangle respectively. Note that the names of the inputs,
         * "Dimension 1" and "Dimension 2" are fixed by the property type and
         * must be such for the property type to operate (it is possible that in
         * the future an alias option could be added so that additional keys,
         * such as "height" and "width" could be associated with each field, but
         * such an option is presently not available).
         */
        change_input("Dimension 1")
          .set_description("The height of the rectangle");
        change_input("Dimension 2")
          .set_description("The width of the rectangle");
    }

private:
    SDE::type::result_map run_(SDE::type::input_map inputs,
                               SDE::type::submodule_map) const override {
        /* Here we implement the module's algorithm. Generally speaking this
         * follows the pattern convert input parameters to variables, compute
         * results, put results in output, return output.
         *
         * The process is somewhat boilerplate heavy so we have implemented
         * convenience functions to help out. For unwrapping the inputs each
         * property type defines a static function unwrap_inputs, which takes
         * the SDE::type::input_map and returns a tuple of values. The order of
         * the returns is the same as the property type's input signature.
         *
         */
        const auto[dim1, dim2] = Area::unwrap_inputs(inputs);

        /* To get our additional input we have to manually unwrap the value. It
         * should be noted that property type unwrap is equivalent to this
         * unwrap, i.e., the above code is equivalent to:
         *
         * auto dim1 = inputs.at("Dimension 1").value<double>();
         * auto dim2 = inputs.at("Dimension 2").value<double>();
         */
        auto name = inputs.at("Name").value<std::string>();

        // With our inputs read in we compute the area
        const auto area = dim1 * dim2;

        /* and assemble the ASCII picture we agreed to write (which only
         * captures the relative sizes of the length and hieight and not the
         * scale because I'm too lazy to code that up).
         */
        //{
        std::stringstream pic;
        pic << name << " :" << std::endl;
        std::array<size_t, 2> dims{10, 10};
        if(dim1 > dim2)
            dims[1] = 5;
        else if(dim2 > dim1)
            dims[0] = 5;

        // Note: std::string(int N, char c) sets string to N copies of c
        std::string top(dims[1], '*');
        std::string space(dims[1] - 2, ' ');
        pic << top << std::endl;
        for(size_t i = 0; i < dims[0] - 2; ++i)
            pic << '*' << space << '*' << std::endl;
        pic << top; // bottom of rectangle
        //}

        /* With our values computed we now need to package them up and return
         * them. Like unwrapping the inputs, each property type provides a
         * static functions wrap_results that will fill up an
         * SDE::type::result_map instance with our computed values. The input to
         * the wrap_results command is the map we are filling in (ultimately the
         * full map is the union of all results so there's often more results
         * than just what one property type defines) and a set of objects
         * containing the values for that property type's results.
         *
         * Like the inputs, we need to manually pack our additional output
         * value. For the record, the wrap_results command is equivalent to the
         * same packing line of code, i.e.:
         *
         * output.at("Area").change(area);
         */
        //{
        auto result = results();
        result      = Area::wrap_results(result, area);
        result.at("Picture").change(pic.str());
        //}

        // Finally we return the map of results
        return result;
    }
};

/* The Prism class implements a module that can compute the volume of a prism
 * by using a submodule. Compared to the Rectangle class, the main additional
 * features showcased by the Prism class are:
 *   - Unwrapping a single input value
 *   - Calling a submodule
 *   - Wrapping multiple return values.
 */
class Prism : public SDE::ModuleBaseHelper<Prism> {
public:
    Prism() {
        // The bulk of the setup ia analogous to Rectangle
        //{
        satisfies_property_type<PrismVolume>();
        description("Computes the area of a prism");
        citation("Euclid. The Elements. 300 BCE");
        change_input("Dimensions")
          .set_description("1st 2 elements are for the base, last is height");
        //}

        /* As we'll see in the run_ member, this class uses a submodule to
         * compute the area of the base. This decouples the algorithm for
         * computing the area of the base from the algorithm for computing the
         * volume of the prism (area of base times height).
         */
        add_submodule<Area>("area").set_description(
          "Submodule used to compute base's area");
    }

private:
    SDE::type::result_map run_(
      SDE::type::input_map inputs,
      SDE::type::submodule_map submods) const override {
        /* Obtaining the inputs for this class is easier than Rectangle because
         * we do not have any additional inputs. Note that there's no need to
         * use structured bindings if there's only one input to unwrap.
         */
        const auto & [dims] = PrismVolume::unwrap_inputs(inputs);

        /* Here we call our submodule. Like the inputs and outpus
         */
        //{
        auto[area]  = submods.at("area").run_as<Area>(dims[0], dims[1]);
        auto volume = area * dims[2];
        //}

        /* Finally we need to wrap and return the results.
         *
         */
        auto out = results();
        return PrismVolume::wrap_results(out, area, volume);
    }
};
