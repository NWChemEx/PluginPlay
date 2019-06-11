/* The purpose of this tutorial is to showcase an end-to-end use of the sde from
 * C++.
 *
 * For all intents an purposes the ``ModuleManager`` class can be thought of as
 * the code embodiment of the sde. It is responsible for storing all of the
 * available modules, assembling the call graph, integrating the modules into
 * the actual runtime, and checkpointing the progress of the program. To get
 * the ``ModuleManager`` class one includes ``ModuleManager.hpp``. The remainder
 * of the following header files are needed for this tutorial.
 */
#pragma once

#include <sde/module_manager.hpp>

/* Running a Computation
 * ---------------------
 *
 * The ``load_modules()`` functions are provided by libraries designed to
 * interact with the sde. You typically will not be instantiating them, rather
 * you will simply be consuming them in your workflow. The following function
 * is an example workflow that a end-user would actually do. Typically it would
 * be run from Python (the contents being nearly identical).
 */
static void example_main() {
    /* Ultimately the run starts by creating and initializing a ``ModuleManger``
     * instance. This is where we leverage the ``load_modules()`` functions
     * provided by libraries .
     */
    sde::ModuleManager mm;
    load_modules(mm);

    /* With the modules loaded it's time to set the options up so that they
     * suite our specific run. For sake of example we'll change the name of
     * our rectangle (even though the name is pointless).
     */
    mm.change_input("Rectangle", "Name", "My Rectangle");

    /* It is worth noting that this is just shorthand for:
     *
     * .. code-block:: c++
     *
     *    mm.at("Rectangle").inputs().at("Name").change("My Rectangle");
     *
     * So if you wanted more control over the changes you could work with the
     * module and input directly.
     *
     * With all of our options set we are ready to run. If we weren't ready to
     * run this function would throw. Note that the PrismVolume class
     * requires a submodule to compute the area of the prism's base. At no point
     * did the developer of the ``Prism`` class specify what that module should
     * be other than it needs to be able to compute an area. It's the
     * ``ModuleManager`` that connects these modules (ultimately calling the
     * ``Rectangle`` module since that's what the default is set to).
     */
    std::vector<double> dims{1.23, 4.56, 7.89};
    auto[volume, area] = mm.run_as<PrismVolume>("Prism", dims);
} // end example_main()

/* That's it. That's the end of the workflow tutorial. From the end-user's
 * perspective a typical run looks like the contents of the ``example_main``
 * function. You load a ``ModuleManager`` instance, set your options, and then
 * run. The sde also provides Python bindings and it is expected that users
 * will usually execute the above in Python. Nonetheless, the ability to run
 * the sde entirely from C++ may be useful for developers looking to implement
 * domain-specific languages on top of it.
 */
