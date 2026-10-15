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

/* The purpose of this tutorial is to showcase the packaging of modules into a
 * plugin.
 *
 * For all intents an purposes the ``ModuleManager`` class can be thought of as
 * the code embodiment of pluginplay. It is responsible for storing all of
 * the available modules, assembling the call graph, integrating the modules
 * into the actual runtime, and checkpointing the progress of the program.
 * Libraries meant for use with pluginplay distribute their modules as a
 * plugin, which the user loads into a ``ModuleManager``. To declare a plugin
 * one includes ``pluginplay/plugin/plugin.hpp`` (included by
 * ``pluginplay/pluginplay.hpp``). The remainder of the following header files
 * are needed for this tutorial.
 */
#pragma once
#include "writing_a_module.hpp"
#include "writing_a_property_type.hpp"
#include <pluginplay/pluginplay.hpp>

/* Declaring the Plugin
 * --------------------
 *
 * A plugin is a class that derives from ``pluginplay::Plugin``. The
 * ``DECLARE_PLUGIN`` macro declares such a class, with a default constructor.
 * In a real library this declaration is the library's public API; users only
 * need it to load the plugin, not the types of your modules.
 */
DECLARE_PLUGIN(ExamplePlugin);

/* Defining the Plugin
 * -------------------
 *
 * Plugins are written like modules: everything is set up in the constructor,
 * which is started with the ``PLUGIN_CTOR`` macro. Normally the constructor is
 * defined in a source file; since this tutorial is a header, it is marked
 * ``inline``.
 *
 * The following constructor assumes that our ``Area``, ``PrismVolume``,
 * ``Rectangle`` and ``Prism`` classes are part of a library meant for use with
 * pluginplay.
 */
inline PLUGIN_CTOR(ExamplePlugin) {
    /* Every plugin has a name. Plugin names must be unique within a
     * ``ModuleManager``.
     */
    set_name("example_plugin");

    /* Next, the plugin adds its modules. Adding a module involves two things:
     *
     * - creation of an instance of the module
     * - association of that instance with a module key
     *
     * The module key is how users refer to the module (*vide infra*). The key
     * must be unique within the plugin (if it's not an error will be raised).
     */
    add_module<Rectangle>("Rectangle");
    add_module<Prism>("Prism");

    /* Our ``Prism`` module needs a submodule which computes an ``Area``. Since
     * our plugin provides one, we set it as the default submodule. Users can
     * still choose a different submodule later.
     *
     * If a default submodule should come from a different plugin, the plugin
     * instead lists that plugin with ``set_preferred_providers`` and records
     * the default with ``default_submod_from``. The ``ModuleManager`` applies
     * those defaults when the user calls ``wire_defaults``.
     */
    change_submod("Prism", "area", "Rectangle");
} // end ExamplePlugin ctor
