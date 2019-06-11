#pragma once
#include "examples/module_base.hpp"
#include "examples/property_type.hpp"

/* Populating the ModuleManager
 * ----------------------------
 *
 * Before the ``ModuleManager`` can manage some modules, it needs modules. The
 * primary means of providing the ``ModuleManager`` with modules is manually.
 * While this sounds tedious, and it is, it only needs to be done once. To this
 * end libraries meant for use with the sde will provide a function
 * ``load_modules`` which takes a ``ModuleManager`` instance and adds to it
 * the modules contained within that library. It is also typically during this
 * function call that default modules are assigned.
 *
 * The following function indicates how this would be done assuming that our
 * ``Area``, ``PrismVolume``, ``Rectangle`` and ``Prism`` classes are part of a
 * library meant for use with the sde.
 */
inline void load_modules(sde::ModuleManager& mm) {
    /* The function starts off by registering the two modules provided by our
     * library. Registration involves two things:
     *
     * - creation of an instance of the module
     * - association of that instance with a module key
     *
     * By taking an already instantiated object, the sde remains decoupled from
     * the procedure for initializing the module. It remains decoupled from the
     * modules' C++ types (here ``Rectangle`` and ``Prism``) by taking the
     * objects via pointers to the ``ModuleBase`` class it necessarily inherits
     * from. To avoid gotchas related to handling polymorphic data types the
     * sde requires that the user provide the module as a ``shared_ptr``. The
     * provided key can be used to retrieve the module (*vide infra*). The key
     * must be unique (if it's not an error will be raised).
     */
    mm.add_module("Rectangle", std::make_shared<Rectangle>());
    mm.add_module("Prism", std::make_shared<Prism>());

    /* Since our modules introduced two new property types we should also set
     * default modules for each of them. These defaults will be used when
     * a module needs, say an ``Area``, and the user has not specifically stated
     * which submodule to use. Setting the defaults for our library is easy
     * since it only provides one of each...
     */
    mm.set_default<Area>("Rectangle");
    mm.set_default<PrismVolume>("Prism");
} // end load_modules()
