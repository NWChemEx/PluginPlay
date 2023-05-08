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

/* This tutorial shows you how to define property types. Recall that a property
 * type defines the API used to compute a property. It is decoupled from the
 * algorithm actually used to compute that property. Over the course of the
 * tutorial we will make two property types:
 *
 * - ``Area`` for computing the area of a shape
 * - ``PrismVolume``, for computing the volume of a Prism.
 *
 * To start, we include the ``property_type.hpp`` header file, which defines the
 * API for registering a new property type:
 */
#pragma once
#include <pluginplay/property_type/property_type.hpp>

/* Declaring the Property Type
 * ---------------------------
 *
 * We start with the ``Area`` property type. This property type is going to
 * define the API for all modules that can compute the area of a shape. In
 * designing this API we try to think of all the possible ways to compute the
 * area of a shape and distill them down to the common elements. For the
 * purposes of this tutorial we simply note that one usually computes the area
 * of a shape from its dimensions such as length of the base. For most shapes we
 * need one or two dimensions to compute the area so we decide that the area
 * property type should take two dimensions, each of type ``double``. The
 * resulting property that all of these modules will compute is the area, which
 * we choose to also represent as a ``double``. Hence our ``Area`` property type
 * defines an API for that looks like:
 *
 * .. code-block::c++
 *
 *    double function_to_compute_area(double, double)
 *
 * With the API designed we now go about encoding it. First we declare the class
 * that will encode it, ``Area``. The class needs to inherit from the
 * ``pluginplay::property_type`` class. ``pluginplay::property_type`` is a
 * templated class that requires one template type parameter, which is the type
 * of the class deriving from it. This is a common pattern in C++ known as the
 * curiously recursive template pattern, or CRTP, that is used to implement
 * static polymorphism. For our purposes, this amounts to the pluginplay seeing
 * the property type API, we are about to define, at compile-time so that
 * type-safety can be enforced. For all intents and purposes the only thing you
 * need to know is that you declare your class like this:
 */
class Area : public pluginplay::PropertyType<Area> {
public:
    /* Declaring the Input API
     * -----------------------
     *
     * The API provided by the pluginplay for registering a property type
     * requires you to separately register the input and result APIs. We start
     * with the input API, but you could also start by declaring the result API.
     * Anyways, the relevant registration requires by having your class declare
     * a function
     * ``inputs_()``. This function must take no arguments and it is strongly
     * recommended that you simply leave the return type as ``auto``. In reality
     * a lot of template meta-programming is about to happen (don't worry it's
     * fairly transparent to you) and you don't want to see the resulting types
     * let alone try to work them out.
     */
    auto inputs_() {
        /* The actual input creation follows a factory pattern. We start by
         * making an instance of the factory object. The type of the object is
         * a bit ugly so the pluginplay provides a convenience function
         * ``pluginplay::declare_input()`` for this purpose. Next we need to add
         * the fields in the order we want them to appear. For example if we
         * want our function to be called like:
         *
         * .. code-block:: c++
         *
         *    f(int, double)
         *
         * We need to first add an integer field and then a double field. To
         * add the fields we note that the factory object defines a member
         * function ``add_field``, which is used to declare a field. The syntax
         * is:
         *
         * .. code-block::c++
         *
         *    add_field<T>("name")
         *
         * where ``T`` is the type of the field, and ``"name"`` is a string name
         * for it (think Python's kwargs).
         *
         * Now the fun part, this is where the template meta-programming will
         * occur. The exact details can be ignored as long as we chain the calls
         * to ``add_field`` (in the order we want the parameters to appear) and
         * capture the return with ``auto``. Hence we declare the ``Area``
         * class's input like:
         */
        auto rv = pluginplay::declare_input()
                    .add_field<double>("Dimension 1")
                    .add_field<double>("Dimension 2");

        /* That's it. Congratulations you just did template meta-programming.
         *
         * The resulting object with the identifier ``rv`` can now be used like
         * a map where the keys are the names of the inputs and the values are
         * the corresponding ``ModuleInput`` instances. Since the property type
         * is more-or-less an abstract base class it usually won't know much
         * about how the inputs will be used. Nonetheless, it is possible to
         * specify some of the metadata, for example these very generic
         * descriptions:
         */
        rv["Dimension 1"].set_description("The length of the 1st dimension");
        rv["Dimension 2"].set_description("The length of the 2nd dimension");

        /* Once we're all done configuring the metadata the only thing left to
         * do is to return the object holding the API
         */
        return rv;
    } // end inputs_()

    /* Declaring the Return API
     * ------------------------
     *
     * Defining the return API follows nearly the same pattern as the inputs.
     * While it should be obvious to readers that a function may take multiple
     * inputs, it is perhaps less obvious that you can return multiple results.
     * To return multiple results, one returns them in an ``std::tuple``.
     * Starting with C++17, C++ can ultimately unpack that tuple for you (the
     * fancy-pants name for this is structured binding). Point being, like the
     * input you can declare multiple returns and you do so in the order you
     * want them to appear. That all said, our ``Area`` API only returns the
     * area so we only need to declare one field:
     */
    auto results_() {
        auto rv = pluginplay::declare_result().add_field<double>("Area");
        rv["Area"].set_description("The area of the shape");
        return rv;
    } // end results_()
};    // end Area

/* This concludes the property type tutorial. We leave you with the
 * definition of the ``Perimeter`` and ``PrismVolume`` classes which we will
 * need in later tutorials, The definitions for these classes are entirely
 * analogous to that of the ``Area`` class. Nonetheless the reader may want to
 * note:
 * - How property types that take the same input should strive to match APIs.
 * - inputs can be constant-references in addition to by-value
 * - the syntax for declaring multiple results
 */
class Perimeter : public pluginplay::PropertyType<Perimeter> {
public:
    auto inputs_() {
        auto rv = pluginplay::declare_input()
                    .add_field<double>("Dimension 1")
                    .add_field<double>("Dimension 2");
        rv["Dimension 1"].set_description("The length of the 1st dimension");
        rv["Dimension 2"].set_description("The length of the 2nd dimension");

        return rv;
    } // end inputs_()

    auto results_() {
        auto rv = pluginplay::declare_result().add_field<double>("Perimeter");
        rv["Perimeter"].set_description("The perimeter of the shape");
        return rv;
    } // end results_()
};    // end Perimeter

class PrismVolume : public pluginplay::PropertyType<PrismVolume> {
public:
    auto inputs_() {
        auto rv =
          pluginplay::declare_input().add_field<const std::vector<double>&>(
            "Dimensions");
        rv["Dimensions"].set_description("The length of each dimension");
        return rv;
    } // end inputs_()

    auto results_() {
        auto rv = pluginplay::declare_result()
                    .add_field<double>("Base area")
                    .add_field<double>("Volume");
        rv["Base area"].set_description("The area of the base");
        rv["Volume"].set_description("The volume of the prism");
        return rv;
    } // end results_()
};    // end PrismVolume
