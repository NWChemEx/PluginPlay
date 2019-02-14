#pragma once
#include "examples/TestModuleBase.hpp"
#include "examples/TestPropertyType.hpp"
#include <SDE/ModuleManager.hpp>

/* Typically a developer does not write just one module, but rather a series of
 * related modules. It is natural to package these modules together in the same
 * library. As part of their library's public API the developer is expected to
 * define a function ``load_modules``, which takes a ModuleManager instance and
 * then proceeds to load each of their library's modules into the instance. They
 * also can set the default module to use for a particular property type.
 *
 * Using our area and volume modules as an example, the following function is an
 * example of how one would write such a function.
 */

static void load_modules(SDE::ModuleManager& mm) {
    /* We start by registering our modules. We associate with each module a
     * module key. This is a unique identifier for the module. Here we
     * simply use the name of the class, but typically you want to ensure it is
     * more unique, for example, by prefixing it with the name of your package.
     */
    mm.add_module("Rectangle", std::make_shared<Rectangle>());
    mm.add_module("Prism", std::make_shared<Prism>());

    /* Since our modules introduced two new property types we should also set
     * default modules for each property type. These defaults will be used when
     * a module needs, say an area, and the user has not specifically stated
     * which module to use.
     */
    mm.set_default<Area>("Rectangle");
    mm.set_default<PrismVolume>("Prism");
}

/* Typically a user drives an SDE calculation from Python; however it is also
 * possible to do this from C++. The following function implements a mock main
 * function showcasing how this works.
 */
static void example_main() {
    // The first thing a user does is create a ModuleManager instance
    SDE::ModuleManager mm;

    /* Next you should populate it with all the modules you want access to.
     * Here we only want access to the example modules, but in practice you may
     * need to call several functions to get all of the modules you want.
     */
    load_modules(mm);

    /* With the modules loaded it's time to tweak the options to our specific
     * run. For sake of example we'll change the name of our rectangle.
     */
    mm.change_input("Rectangle", "Name", "My Rectangle");

    /* It is worth noting that this is just shorthand for:
     *
     * mm.at("Rectangle").inputs().at("Name").change("My Rectangle");
     *
     * if you want more control over the changes you can work with the module
     * directly.
     */

    /* With all of our options set we are ready to run. If we weren't ready to
     * run this function would throw xxx. Note that the PrismVolume class
     * requires a submodule to compute the area of the prism's base. The
     * ModuleManager satisfies this requirement for us automatically by
     * providing the PrismVolume module whatever module is registered as the
     * default for the Area property type (here the module with the key
     * Rectangle).
     */
    std::vector<double> dims{1.23, 4.56, 7.89};
    auto[volume, area] = mm.run_as<PrismVolume>("Prism", dims);

    /* Like it was for setting the options, this is just a convenience function
     * wrapping:
     *
     * auto [area] = mm.at("Rectangle").run_as<Area>(1.23, 4.56);
     *
     * Hence if more control is desired you can work directly with the module to
     * run.
     */
}
