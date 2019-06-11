// TUTORIAL
//
// This tutorial is the C++ analog of the Python Workflow tutorial. Generally
// speaking one will interact with the SDE through Python; however, this is not
// required and there may be situations where one prefers to interact with SDE
// purely from C++.
//
// Like the Python tutorial, we assume you have installed SDE and your compiler
// knows where to find it. If this is the case, then to include SDE in your C++
// project it suffices to include the convenience header file ``sde/sde.hpp```.
#include <sde/sde.hpp>
// TUTORIAL_START_SKIP
#include "packaging.hpp"
#include <catch2/catch.hpp>

TEST_CASE("C++ workflow") {
    // TUTORIAL_STOP_SKIP

    // TUTORIAL
    //
    // Again our first step is to populate the ModuleManager with modules. To
    // that end we create a ModuleManager instance and then add some pre-defined
    // modules. By convention, libraries that provide modules for use with the
    // SDE will provide a ``load_modules`` function. We'll show you how to write
    // the ``load_modules`` function in a later tutorial, for now just focus on
    // the fact this function loads some pre-defined modules including a module
    // capable of computing the area of rectangles.
    sde::ModuleManager mm;
    load_modules(mm);

    // TUTORIAL
    //
    // Like the Python tutorial the module we are interested in is loaded into
    // the ModuleManager under the module key "Rectangle" and is capable of
    // being  used as an ``Area`` property type. The syntax for computing the area
    // of a  rectangle, that is 1.23 by 4.56 is nearly the same as the Python API:
    auto area = mm.run_as<Area>("Rectangle", 1.23, 4.56);

    // TUTORIAL
    //
    // As this tutorial showed the C++ and Python APIs for the SDE are nearly
    // identical. The primary reason to prefer the Python API is that it
    //"doesn't" require recompiling every time you want to change the module you
    // are using ("doesn't" is in quotes because Cppyy actually does
    // just-in-time  compilation while Python is running, but you don't need to
    // know that...).

    // TUTORIAL_START_SKIP
    REQUIRE(area == 1.23 * 4.56);
}
// TUTORIAL_STOP_SKIP
