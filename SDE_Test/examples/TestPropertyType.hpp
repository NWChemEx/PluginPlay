#pragma once
#include <SDE/PropertyType.hpp>

/* This file defines two property types for our SDE example. They are meant
 * purely for illustrative purposes (there's much better ways of defining APIs
 * for computing the area and higher-order analogs). The header file will be
 * used again for later examples.
 */

/* The Area property type is very basic. For all intents and purposes it defines
 * an API:
 *
 * double run(double, double)
 *
 * where the return is the area of the shape and the two inputs are the
 * dimensions of the shape (exactly what these two values are, say whether they
 * are base and height of a triangle or length and width of a rectangle, is
 * left up to the module). The class itself shows:
 *
 *  - Declares the fields in the API for the input/output of modules satisfying
 *    this property type, i.e., it says all modules computing areas take two
 *    doubles and return a double.
 *    - The input API is declared by implementing the "virtual" function (the
 *      function is polymorphic, but compile-time polymorphic, meaning there
 *      is no corresponding "virtual" function in the base class) inputs_()
 *      and the output API is declared via the results_() function
 *  - Inputs can be copies/values
 *
 */
class Area : public SDE::PropertyType<Area> {
public:
    auto inputs_() {
        /* This line declares the object holding the details of the input API.
         * Behind the scenes there is template metaprogramming occurring that
         * is assembling the actual API which requires all the calls to
         * add_field to be chained. The actual return type will in general be a
         * mess so it is strongly recommended that you use auto to spare
         * yourself that detail.
         */
        auto rv = declare_input()
                    .add_field<double>("Dimension 1")
                    .add_field<double>("Dimension 2");

        /* The above could be written as:
         *
         * auto rv1 = declare_input();
         * auto rv2 = rv1.add_field<double>("Dimension 1");
         * auto rv  = rv2.add_field<double>("Dimension 2");
         *
         * and rv would be identical. The result of each call is a different
         * object, which has a different type (basically we encode the
         * additional API details into the new type). This means that:
         *
         * auto rv = declare_input();
         * rv.add_field<double>("Dimension 1");
         * rv.add_field<double>("Dimension 2");
         *
         * or
         *
         * auto rv = declare_input();
         * rv = rv.add_field<double>("Dimension 1");
         * rv = rv.add_field<double>("Dimension 2");
         *
         * will not work as intended. The first one will compile, but creates an
         * API with no fields because the new fields are stored in the returned
         * object, which is thrown away. The second example will fail to
         * compile because the return types of add_field are different than
         * the type of rv.
         */

        /* With the API defined we are free to assign default metadata to each
         * field. For example we provide very generic documentation strings for
         * each of the inputs. Modules implementing this property type are free
         * to make the metadata more useful (e.g., specify that the 1st
         * dimension will be taken as the width of the shape).
         */
        rv["Dimension 1"].set_description("The length of the 1st dimension");
        rv["Dimension 2"].set_description("The length of the 2nd dimension");

        // Finally we return the object storing the API
        return rv;

        /* I admittedly am not sold on the former syntax. It is also
         * possible to have a syntax like:
         *
         * return declare_input().
         *   add_field(
         *      add_input<double>("Dimension 1").set_description("...")
         *   ).
         *   add_field(
         *      add_input<double>("Dimension 2").set_description("...")
         *   );
         *
         *  This syntax avoids some of the redundancy in the first syntax (the
         *  need to specify the name of the field, i.e., "Dimension 1" or
         *  "Dimension 2", twice. It is equivalent to:
         *
         *  auto input1 = add_input<double>("Dimension 1").set_description(...);
         *  auto input2 = add_input<double>("Dimension 2").set_description(...);
         *
         *  return declare_input().add_field(input1).add_field(input2);
         */
    }

    auto results_() {
        /* Implementing the return part of the API is nearly identical to the
         * input API. We create an object with a nasty type (the type of which
         * we ignore by using auto) and then fill in the metadata for each field
         * in the return. Here we have a single return, PrismVolume will
         * showcase that it is possible to have multiple returns as well.
         *
         * All of the syntax comments of inputs_() are relevant here too. In
         * particular it is necessary to chain the add_field() function calls.
         */
        auto rv = declare_result().add_field<double>("Area");
        rv["Area"].set_description("The area of the shape");
        return rv;
    }
};

/* The PrismVolume property type is more complicated than that of the Area
 * property type. It defines an API:
 *
 * std::tuple<double, double> run(const std::vector<double>&)
 *
 * where the returns respectively are the area of the bases and the volume of
 * the prism (the former being returned simply to ensure we have a second
 * return). The input is a read-only vector of dimensions, the interpretations
 * of each element are again left to the module. This class showcases:
 *
 * - Inputs can be read-only references that avoid copies (we prohibit
 *   read/write references as input parameters to mitigate against memoization
 *   breakdowns)
 * - Property types can return multiple values (assuming the result is used
 *   with C++17's structured bindings or C++11's std::tie)
 */
class PrismVolume : public SDE::PropertyType<PrismVolume> {
public:
    auto inputs_() {
        /* With the exception of declaring the field as a read-only reference,
         * the implementation of PrismVolume::inputs_ displays no new features
         * from Area::inputs_
         */
        auto rv =
          declare_input().add_field<const std::vector<double>&>("Dimensions");
        rv["Dimensions"].set_description("The length of each dimension");
        return rv;
    }

    auto results_() {
        /* With the exception of showcasing how to declare multiple return
         * values, the implementation of PrismVolume::results_ displays no new
         * features from Area::results_
         */
        auto rv = declare_result()
                    .add_field<double>("Base area")
                    .add_field<double>("Volume");
        rv["Base area"].set_description("The area of the base");
        rv["Volume"].set_description("The volume of the prism");
        return rv;
    }
};
