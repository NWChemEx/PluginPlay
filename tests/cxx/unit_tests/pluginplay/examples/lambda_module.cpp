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

// TUTORIAL_START_SKIP
#include <catch2/catch.hpp>
// TUTORIAL_STOP_SKIP

// TUTORIAL
//
// This tutorial shows you how to write a lambda module. Lambda modules satisfy
// a similar use case as native C++ lambda functions, namely to facilitate the
// generation of module at runtime. This is useful in a number of scenarios such
// as:
//
// - rapid prototyping
//   - allows you to quickly test a module without having to go through all the
//     rigmarole of defining a full-fledged module
// - unit testing
//   - When unit testing a module that depends on a submodule we can use a
//     lambda module that returns a hard-coded value so that we can decouple the
//     test from the submodule implementation.
// - piggy-backing off of other modules
//   - A lot of methods can be implemented by tweaking the results of a more
//     complicated module.
// - user-input
//   - sometimes it's just easier for the user to define a lambda module that
//     does what they want, than for the developer to try to concoct an API
//     which is flexible enough to account for all possible inputs.
//   - a prototypical example is selecting elements from a set. Often users
//     will want selections like "give me elements 1 through 9, 12, and 15". In
//     Python this can easily be written as a lambda via:
//     ``x = lambda : Range(1,10) + [12, 15]``, but coming up with a
//     conventional API that is as flexible and succinct is difficult.
//
// For the purposes of this tutorial we reuse the property types from our
// property type tutorial and the modules from our module tutorial. The next
// two lines include the relevant header files
#include "workflow.hpp"
#include <pluginplay/pluginplay.hpp>

// TUTORIAL_START_SKIP
TEST_CASE("How to use a lambda module") {
    // TUTORIAL_STOP_SKIP

    // TUTORIAL
    //
    // Our property type tutorial defined three property types: ``Area``,
    // ``Perimeter``, and ``PrismVolume``. Presently we are only interested in
    // the
    // ``Area`` and ``PrismVolume`` property types. ``Area`` defines the API a
    // module must satisfy if it is capable of computing the area of a
    // two-dimensional shape and ``PrismVolume`` defines the API a module must
    // satisfy if it is capable of computing the volume of a prism. In our
    // module tutorial we wrote two modules: ``Rectangle`` that can compute the
    // area (and perimeter) of a rectangle and ``Prism`` which can compute the
    // volume of a prism by calling a submodule to compute the area of the
    // prism's base. Using these modules we can compute the area and perimeter
    // of a rectangle and the volume of a rectangular prism, but what if we
    // wanted to compute the volume of a triangular prism with our ``Prism``
    // module? This ultimately requires a module that can compute the area of a
    // triangle.
    //
    // For the purposes of this tutorial we note that given the length of the
    // base, :math:`b`, and the height, :math:`h`, of a triangle, :math:`T`, we
    // can create a corresponding rectangle, :math:`R`, with sides of lengths
    // :math:`b` and :math:`h` such that the area of the rectangle, :math:`a_R`,
    // is:
    //
    // .. math::
    //
    //    a_R = bh
    //
    // :math:`a_R` is related to the area of our triangle, :math:`a_T`, via:
    //
    // .. math::
    //
    //    a_T = \frac{a_R}{2}
    //
    // Our strategy is thus to piggy-back off of the already existing
    // ``Rectangle`` module and simply divide the result by two.
    //
    // We start with some set-up, which declares and fills a ``ModuleManager``
    // instance and then retrieves the relevant modules for this tutorial:

    pluginplay::ModuleManager mm;
    load_modules(mm);

    // At this point volume_mod's "area" submodule is set to Rectangle
    auto volume_mod = mm.at("Prism");

    // TUTORIAL
    //
    // Now that we have the modules we need we write the lambda module; this is
    // done by using the ``make_lambda`` function. The input to the
    // ``make_lambda`` function is the property type that the lambda module
    // satisfies (here ``Area``) and a callback (typically a C++ lambda, but can
    // be a functor, or normal function) which implements the lambda module. The
    // API of the callback must be the same as property type it satisfies (the
    // ``Area`` property type takes the dimensions of the shape as two
    // ``double``s, and returns the area of the shape, also as a ``double``).
    //
    // In a typical piggy-backing or rapid-prototyping scenario we are in the
    // body of a module and we want to change the behavior of a submodule's
    // submodule. To this end we pretend that `volume_mod` is a submodule that
    // has been passed to our module. The following code intercepts the
    // ``Rectangle`` submodule passed to the ``Prism`` submodule, replaces it
    // with a lambda module, and runs the
    // ``Prism`` submodule.

    {
        // Get the Rectangle submodule from the Prism submodule
        auto area_mod = volume_mod.submods().at("area");

        // Make the lambda module that wraps it
        auto l = pluginplay::make_lambda<Area>([&](double b, double h) {
            return 0.5 * area_mod.run_as<Area>(b, h);
        });

        // Tell the Prism submodule to use our lambda module
        volume_mod.change_submod("area", std::move(l));

        // Run the Prism submodule (note first two elemenst are base's
        // dimensions)
        std::vector<double> dims{1.0, 2.0, 3.0};
        auto [area, volume] = volume_mod.run_as<PrismVolume>(dims);
        assert(std::abs(volume - 3.0) < 1.0E-10);
    }

    // TUTORIAL
    //
    // In addition to piggy-backing off of already existing modules lambda
    // modules are also useful for unit testing. Here, for example, we want to
    // make sure that our ``Prism`` module works correctly regardless of the
    // module it uses to compute the area. In this scenario we write one (or
    // more) lambda module(s) which simply return area values; we then test to
    // ensure our ``Prism`` module returns the correct volume.
    {
        auto l =
          pluginplay::make_lambda<Area>([&](double, double) { return 3.0; });
        auto volume_mod2 = volume_mod.unlocked_copy();

        volume_mod2.change_submod("area", std::move(l));
        std::vector<double> dims{1.0, 2.0,
                                 3.0}; // First two elements are not used
        auto [area, volume] = volume_mod2.run_as<PrismVolume>(dims);
        assert(std::abs(volume - 9.0) < 1.0E-10);
    }

    // TUTORIAL_START_SKIP
}
// TUTORIAL_STOP_SKIP
