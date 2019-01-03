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
        return add_input<double>("Dimension 1")
          .description("The length of the first dimension")
          .add_input<double>("Dimension 2")
          .description("The length of the second dimension");
    }

    auto outputs_() {
        return add_output<double>("Area").description("The area of the shape");
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
        return add_input<const std::vector<double>&>("Dimensions")
          .description("The length of each dimension");
    }

    auto outputs_() {
        return add_output<double>("Base area")
          .description("The area of the base")
          .add_output<double>("Volume")
          .description("The volume of the prism");
    }
};
