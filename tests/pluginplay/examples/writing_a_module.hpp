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

/* This tutorial focuses on how to write a module. It builds upon the tutorial
 * on PropertyTypes by showing how one could go about implementing a module that
 * computes the area and perimeter of a rectangle (the ``Rectangle`` class
 * below) and another module that computes the volume of a prism using a
 * submodule of property type ``Area`` (the ``Prism`` class below).
 *
 * To register a module with the pluginplay you need to include the
 * ``ModuleBase.hpp`` header file. The remainder of the following header list is
 * required for running this tutorial.
 */
#pragma once
#include "writing_a_property_type.hpp"
#include <pluginplay/module_base.hpp>

/* Declaring the Module
 * --------------------
 *
 * The Rectangle module implements the algorithm for computing the area and
 * perimeter of a rectangle. For the record, given a rectangle with a base of
 * length :math:`b` and a height of length :math:`h`, the area, :math:`a`, is
 * given by:
 *
 * .. math::
 *
 *    a = bh
 *
 * and the perimeter, :math:`p`, is given by:
 *
 * .. math::
 *
 *   p = 2(b + h)
 *
 * To begin we inherit from ``pluginplay::ModuleBase``, which is the base class
 * for all module implementations.
 *
 * .. note::
 *
 *    Modules do not inherit from the property types they satisfy.
 */
class Rectangle : public pluginplay::ModuleBase {
public:
    /* Defining the Module's API
     * -------------------------
     *
     * Next we declare the module's constructor, or ctor, for short. Like
     * any other C++ class, the ctor of a module is charged with creating the
     * module and setting up its default state. The default state for a module
     * determines its API, specifically:
     *
     * - the inputs it recognizes
     * - the values it can compute
     * - what properties it needs other modules to compute for it
     *
     * The recommended best practice for establishing a module's state is to do
     * it in the default ctor. Regardless, of the signature of the module's
     * ctor, it needs to explicitly invoke the ``ModuleBase`` ctor.
     * ``ModuleBase`` only has one ctor and it takes your class's ``this``
     * pointer (it's used to get the runtime type information of your class).
     * Point being the best-practice ctor is declared like:
     *
     */
    Rectangle() : pluginplay::ModuleBase(this) {
        /* Most modules will be designed with a property type in mind. For
         * example this module was designed with the explicit intent of making
         * a module that computes areas and perimeters of rectangles. In order
         * to satisfy these property types all of the input and result fields of
         * the ``Area`` and ``Perimeter`` classes must also be fields of this
         * Module. For the ``Area`` and ``Perimeter`` property types this means
         * we'd minimally need to write something like:
         *
         * .. code-block:: c++
         *
         *    add_input<double>("Dimension 1").set_description(...);
         *    add_input<double>("Dimension 2").set_description(...);
         *    add_output<double>("Area").set_description(...);
         *    add_output<double>("Perimeter").set_description(...);
         *
         * All of this information is available in the property types, and the
         * pluginplay can automatically add it to your module if you like. To do
         * this one uses the ``satisfies_property_type`` member function
         * provided by the base class. The signature is:
         *
         * .. code-block:: c++
         *
         *    void satisfies_property_type<T>();
         *
         * where ``T`` is the C++ type of the property type your class
         * satisfies. For our present module that amounts to:
         */
        satisfies_property_type<Area>();
        satisfies_property_type<Perimeter>();

        /* In addition to the metadata associated with each input and output
         * there is also metadata associated with the module itself. Here we
         * set the module's documentation description and add citations for any
         * relevant literature (disclaimer: I have no idea who first worked out
         * the area of a rectangle, but Euclid seems like a good guess).
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
         * recomputed if they are needed later).
         *
         * Anyways, we add an additional input that names our rectangle and an
         * additional result which is a text summary of the result.
         */
        add_input<std::string>("Name")
          .set_description("The name of the rectangle")
          .set_default("");

        add_result<std::string>("Summary").set_description(
          "A text summary of the results");

        /* .. note::
         *
         *    The requirements for adding new inputs and results to a module are
         *    not as stringent as they are for adding them to a property type.
         *    That is because the Module's API is kwargs-like and not
         *    position-based.
         *
         * Particularly when a field is set from a property type the metadata
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
     * ``ModuleBase`` base class. Its signature is:
     *
     * .. code-block:: c++
     *
     *    pluginplay::type::result_map run_(pluginplay::type::input_map inputs,
     *                               pluginplay::type::submodule_map submods)
     * const;
     *
     * The parameters are the values of the inputs your module should use
     * (the pluginplay guarantees that all non-optional options have been set
     * and that the values they are set to have passed all provided bounds
     * checks). In the same vein, the provided map of submodules are the
     * submodules that your module should use (again the pluginplay guarantees
     * that all submodule requests have been satisfied, and that the submodules
     * are ready to be run as the requested property type).
     *
     */
    pluginplay::type::result_map run_(
      pluginplay::type::input_map inputs,
      pluginplay::type::submodule_map) const override {
        /* Typically the first thing done inside a module is to unwrap the
         * inputs. Each property type defines a convenience function
         * ``unwrap_inputs``, which will automatically unwrap its inputs into
         * identifiers for you. The output is an ``std::tuple`` of that property
         * type's input parameters, in the order they were defined. For our
         * present example recall that the ``Area`` property type declared its
         * parameters like:
         *
         * .. code-block:: c++
         *
         *    auto rv = declare_input()
         *              .add_field<double>("Dimension 1")
         *              .add_field<double>("Dimension 2")
         *
         * Hence the values we get back will be ``"Dimension 1"`` and
         * ``"Dimension 2"`` in that order.
         */
        const auto [base, height] = Area::unwrap_inputs(inputs);

        /* In our present case, the ``Perimeter`` class did not define any new
         * inputs compared to the ``Area`` class, which means the next line is
         * superfluous; however, generally speaking, when a module satisfies
         * multiple property types you will need to unwrap the inputs from each.
         *
         * .. note::
         *
         *    As of C++17, C++ does not support "skipping" a value when using
         *    structured bindings. Thus, even if you do not need a value you
         *    will need to provide a placeholder for it.
         */
        const auto [base1, height1] = Perimeter::unwrap_inputs(inputs);

        /* For inputs that are not part of a property type you will need to
         * grab them manually. The syntax is:
         */
        auto name = inputs.at("Name").value<std::string>();

        /* With our inputs read in we actually implement the algorithm, which
         * in this case means we compute the area and perimeter. We also make
         * the string summary.
         */
        const auto area      = base * height;
        const auto perimeter = 2 * (base + height);
        const auto summary = name + " has an area of " + std::to_string(area) +
                             " and a perimeter of " + std::to_string(perimeter);

        /* With our values computed we now need to package them up and return
         * them. Like unwrapping the inputs, each property type provides a
         * static function ``wrap_results`` that will fill up an
         * pluginplay::type::result_map instance with our computed values. The
         * input to the wrap_results command is the map we are filling in and
         * the values for the results, in the order specified in the property
         * type. containing the values for that property type's results.
         */
        auto result = results();
        result      = Area::wrap_results(result, area);
        result      = Perimeter::wrap_results(result, perimeter);

        /*
         * Packaging of results that are not part of a property type must be
         * done manually.
         */
        result.at("Summary").change(summary);
        return result;
    }
}; // end Rectangle class

/* The implementaion of the ``Rectangle`` class covered most of the details
 * pertaining to implementing a module. Perhaps the only thing missing is how to
 * use submodules. The ``Prism`` class implements a module that can compute the
 * volume of a prism. For the record, the volume, :math:`V`, of a prism of
 * height, :math:`h`, whose base has an area of :math:`a` is given by:
 *
 * .. math::
 *
 *    V = ah
 *
 * We could code up multiple ``Prism`` modules for each possible base, or we
 * can simply encapsulate that detail by calling another module, we opt for the
 * latter approach here.
 *
 * The bulk of the ``Prism`` class is analogous to the ``Rectangle`` class:
 */
class Prism : public pluginplay::ModuleBase {
public:
    Prism() : pluginplay::ModuleBase(this) {
        satisfies_property_type<PrismVolume>();
        description("Computes the area of a prism");
        citation("Euclid. The Elements. 300 BCE");
        change_input("Dimensions")
          .set_description("1st 2 elements are for the base, last is height");
        //}

        /* The new feature is we need to register that our module has one call
         * back point. We do this via the ``add_submodule`` member of
         * ``ModuleBase``. The signature of which is:
         *
         * .. code-block:: c++
         *
         *    SubmoduleRequest add_submodule<T>("key");
         *
         * where ``T`` is the C++ type of the associated property type, `"key"`
         * is the name to associate with the callback point (used to distinguish
         * between callback points in a module), and the return is an object
         * managing the metadata associated with the request.
         *
         * For our ``Prism`` class we need a module that compute an area, the
         * property type for which is ``Area``. We associate with this callback
         * point the key ``"area"`` and additionally set the description of the
         * callback point. The result is:
         */
        add_submodule<Area>("area").set_description(
          "Submodule used to compute base's area");

        /* We now proceed to finish defining the ``Prism`` class, The majority
         * of this definition is completely analogous to the ``Rectangle`` class
         * until we need to call the submodule.
         */

    } // end Prism()

private:
    pluginplay::type::result_map run_(
      pluginplay::type::input_map inputs,
      pluginplay::type::submodule_map submods) const override {
        const auto& [dims] = PrismVolume::unwrap_inputs(inputs);

        /* Like inputs, the submodules to use will be provided to your module's
         * ``run_`` function at runtime. You retrieve the relevant submodule
         * using the name you associated with it, in this case `"area"``. With
         * the submodule in hand you then run it via the appropriate property
         * type API. It is worth noting that this call is completely type-safe,
         * which is to say you will get an error at compile time if you try to
         * pass an argument into ``run_as`` that is of the incorrect type.
         * Similarly, you will get an error if you try to assign the result of
         * the call to an identifier of the wrong type.
         */
        auto area = submods.at("area").run_as<Area>(dims[0], dims[1]);

        /* This concludes the module tutorial. The remainder of the code
         * finishes the implementation of the ``Prism`` class and presents
         * nothing new.
         */
        auto volume = area * dims[2];

        auto out = results();
        return PrismVolume::wrap_results(out, area, volume);
    } // end run_()
};    // end Prism
