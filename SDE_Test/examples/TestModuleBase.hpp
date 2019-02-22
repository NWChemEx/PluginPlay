/* This tutorial focuses on how to write a module. It builds upon the tutorial
 * on PropertyTypes by showing how one could go about implementing a module that
 * computes the area of a rectangle (the ``Rectangle`` class below) and another
 * module that computes the volume of a prism using a module of property type
 * ``Area`` (the ``Prism`` class below).
 *
 * To run the code in this tutorial you will need the following includes:
 */
#pragma once
#include "examples/TestPropertyType.hpp"
#include <SDE/ModuleBase.hpp>
#include <sstream>

/* Declaring the Module
 * --------------------
 *
 * The Rectangle module implements the algorithm for computing the area of a
 * rectangle. For the record, given a rectangle with a base of length :math:`b`
 * and a height of length :math:`h`, the area, :math:`a`, is given by:
 *
 * .. math::
 *
 *    a = bh
 *
 * To begin we inherit from ``SDE::ModuleBaseHelper<T>`` where ``T`` is replaced
 * with the type of the class you are implementing. We saw this pattern before
 * in the PropertyType tutorial, it's the curiously recurring template pattern,
 * and it is used here for the same purpose, to have the SDE automatically
 * implement some functionality for you using your class's type. Regardless, the
 * important thing to note is that all modules derive from
 * ``SDE::ModuleBaseHelper<T>``, where ``T`` is the name of the module's type.
 */
class Rectangle : public SDE::ModuleBase {
public:
    /* Defining the Module's API
     * -------------------------
     *
     * Next we declare the module's constructor, or ctor, for short. Like for
     * any other C++ class, the ctor of a module is charged with creating the
     * module and setting up its default state. The default state for a module
     * determines its API, specifically:
     *
     * * the inputs it recognizes
     * * the values it can compute
     * * what properties it needs other modules to compute for it
     *
     * You're free to define the ctor any way you like, because the SDE takes
     * and uses an already created instance of your class. That said since the
     * default state for a module should be the same for all instances it is
     * recommended that developers keep things simple and implement it in a
     * default ctor like:
     */
    Rectangle() : SDE::ModuleBase(this) {
        /* Most modules will be designed with a property type in mind. For
         * example this module was designed with the explicit intent of making
         * a module that computes areas. In order to satisfy a property
         * type all of the input and output fields of that property type must be
         * included in the module's sets of input and output fields. For the
         * ``Area`` property type that means we'd minimally need to write
         * something like:
         *
         * .. code-block:: c++
         *
         *    add_input<double>("Dimension 1").set_description(...);
         *    add_input<double>("Dimension 2").set_description(...);
         *    add_output<double>("Area").set_description(...);
         *
         * All of this information is available in the property type, so there's
         * not actually a need to repeat it here. To automatically add the
         * inputs and results from a property type we use the
         * ``satisfies_property_type`` member function of the base class.
         */
        satisfies_property_type<Area>();

        /* .. note::
         *
         *    If a module satisfies multiple property types you simply repeat
         *    this call for each of the property types.
         *
         * In addition to the metadata associated with each input and output
         * there is also metadata associated with the module itself. Here we
         * set the module's documentation description and add citations for any
         * relevant literature.
         */
        description("Computes the area of a rectangle");
        citation("Euclid. The Elements. 300 BCE");

        /* The input/results of a module are not limited to those specified by
         * the property type. You are free to add additional inputs and results.
         * Keep in mind that these additional fields will not be accessible from
         * the property type API, which means that end-users will need to bind
         * the additional inputs before calling your module. Nevertheless the
         * additional inputs/results can always be accessed via the expert
         * API and may be accessible from other property types. This means that
         * particularly for inputs it is best if these additional inputs are
         * algorithmic parameters with default values (additional results can
         * always be ignored no problem and thanks to memoization need not be
         * recomputed if they are later needed).
         *
         * Anyways, we add an additional input that names our rectangle (perhaps
         * for printing) and an additional output which is an ASCII picture of
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
    } // end Rectangle()

private:
    /* Implementing the Module's Algorithm
     * -----------------------------------
     *
     * Now that we defined the module's API we have to actually implement the
     * algorithm it encapsulates. The algorithm goes in the ``run_`` member. The
     * ``run_`` member is a pure abstract member function defined by the
     * ``ModuleBase`` base class. It's signature is:
     *
     * .. code-block:: c++
     *
     *    SDE::type::result_map run_(SDE::type::input_map inputs,
     *                               SDE::type::submodule_map submods) const;
     *
     * The parameters are the values of the inputs your module should use
     * (the SDE guarantees that all non-optional options have been set and that
     * the values they are set to have passed all provided bounds checks). In
     * the same vein, the provided map of submodules are the submodules that
     * your module should use (again the SDE guarantees that all submodule
     * requests have been satisfied, and that the submodules are ready to be run
     * as the requested property type).
     *
     */
    SDE::type::result_map run_(SDE::type::input_map inputs,
                               SDE::type::submodule_map) const override {
        /* Typically the first thing done inside a module is to unwrap the
         * inputs. Each property type defines a convenience function
         * ``unwrap_inputs``, which will automatically unwrap it's inputs into
         * identifiers for you. The output is an ``std::tuple`` of that property
         * type's input parameters in the order they were defined. For our
         * present example recall that the ``Area`` property type declared its
         * parameters like:
         *
         * .. code-block::
         *
         *    auto rv = declare_input()
         *              .add_field<double>("Dimension 1")
         *              .add_field<double>("Dimension 2")
         *
         */
        const auto[dim1, dim2] = Area::unwrap_inputs(inputs);

        /* Since the
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
}; // end Rectangle class

/* The Prism class implements a module that can compute the volume of a prism
 * by using a submodule. Compared to the Rectangle class, the main additional
 * features showcased by the Prism class are:
 *   - Unwrapping a single input value
 *   - Calling a submodule
 *   - Wrapping multiple return values.
 */
class Prism : public SDE::ModuleBase {
public:
    Prism() : SDE::ModuleBase(this) {
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
}; // end Prism
