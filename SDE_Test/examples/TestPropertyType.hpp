#pragma once
#include <SDE/PropertyType.hpp>

/* This file defines two property types for our SDE example. They are meant
 * purely for illustrative purposes (there's much better ways of defining APIs
 * for computing the area and higher-order analogs).
 */

/* The Area property type is very basic. For all intents and purposes it defines
 * an API:
 *
 * double run(double, double)
 *
 * where the return is the area of the shape and the two inputs are the
 * dimensions of the shape (exactly what these two values are, say whether they
 * are base and height of a triangle or length and width of a rectangle, is
 * left up to the module).
 *
 */
class Area : public SDE::PropertyType<Area> {
public:
    auto inputs_() {
        auto rv = declare_input()
                    .add_field<double>("Dimension 1")
                    .add_field<double>("Dimension 2");
        rv["Dimension 1"].set_description("The length of the 1st dimension");
        rv["Dimension 2"].set_description("The length of the 2nd dimension");
        return rv;
    }

    auto outputs_() {
        auto rv = declare_output().add_field<double>("Area");
        rv["Area"].set_description("The area of the shape");
        return rv;
    }
};

/* The PrismVolume property type is more complicated than that of the Area
 * property type. It defines an API:
 *
 * double, double run(const std::vector<double>&)
 *
 * where the returns respectively are the area of the bases and the volume of
 * the prism. The input is a read-only vector of dimensions, the interpretations
 * of which is again left to the module.
 */
class PrismVolume : public SDE::PropertyType<PrismVolume> {
public:
    auto inputs_() {
        auto rv =
          declare_input().add_field<const std::vector<double>&>("Dimensions");
        rv["Dimensions"].set_description("The length of each dimension");
        return rv;
    }

    auto outputs_() {
        auto rv = declare_output()
                    .declare_field<double>("Base area")
                    .declare_field<double>("Volume");
        rv["Base area"].set_description("The area of the base");
        rv["Volume"].set_description("The volume of the prism");
        return rv;
    }
};
