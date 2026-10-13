.. Copyright 2022 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

################
Writing a Plugin
################

A module collection is distributed as a plugin. When a user starts a run with
PluginPlay one of the first things they do is initialize a ``ModuleManager``
instance and load the plugins they want to use. The code to do this looks
something like:

.. code-block:: c++

   // Initialize ModuleManager
   ModuleManager mm;
   mm.load_plugin<library1::Library1>();
   mm.load_plugin<library2::Library2>();
   mm.wire_defaults();

where the ``load_plugin`` calls load the plugins provided by the various
libraries and ``wire_defaults`` sets the default submodules the plugins want
from each other (see `Defaults From Other Plugins`_).

Public API
==========

As far as the public API for a plugin goes, the declaration is simple:

.. literalinclude:: ../../../../tests/cxx/doc_snippets/pluginplay_examples.hpp
   :language: c++
   :lines: 17-

``DECLARE_PLUGIN`` declares a class (here ``PluginplayExamples``) that derives
from ``pluginplay::Plugin`` and has a default constructor. Note that it is not
necessary for the user to know the types of your modules and more importantly
any libraries you use to implement them.

.. note::

   If your modules use property types defined elsewhere (such as in centralized
   community resources like NWChemEx/SimDE) and your
   module-specific inputs are standard C++ types (or types found in the
   property types) this is your entire public API!!!

Implementation
==============

Plugins are written like modules: everything is set up in the plugin's
constructor, which is started with the ``PLUGIN_CTOR`` macro. The constructor
typically does three things: names the plugin, adds the plugin's modules, and
sets the default submodules.

Every plugin has a name, set by calling ``set_name``. Plugin names must be
unique within a ``ModuleManager``. Like module keys, plugin names are
case-insensitive.

Modules are added to the plugin by calling ``add_module``. For your
convenience ``add_module`` has an overload which takes a template type
parameter that is the type of your module (as in the name of the class which
contains the algorithm) and a module key. The module key is how users will
refer to your module, *e.g.*, when they want to run your module they will call
``ModuleManager::run_as`` providing your module's key. Module keys must be
unique within your plugin. If your module requires more detailed set-up other
overloads exist that will bind to existing instances.

Continuing with our electric field examples, naming our plugin and adding our
modules looks like:

.. literalinclude:: ../../../../tests/cxx/doc_snippets/pluginplay_examples.cpp
   :language: c++
   :lines: 17-29

Once you have added all your modules the next step is to set the default
submodules. In our examples, the ``ClassicalForce`` module needed a submodule
which can compute electric fields. Presently we have already loaded three such
modules; their keys are:

- "Coulomb's Law"
- "Coulomb's Law with screening"
- "Single-precision Coulomb's law"

If by default we want the ``ClassicalForce`` module we loaded under the key
"Classical Force" to use a ``CoulombsLaw`` module we can accomplish this by:

.. literalinclude:: ../../../../tests/cxx/doc_snippets/pluginplay_examples.cpp
   :language: c++
   :lines: 31

.. note::

   It is not necessary to set all submodules in the plugin's constructor.
   In particular if your module depends on a property that none of your modules
   can compute you would not set that submodule. In this case, the submodule
   can come from another plugin (see `Defaults From Other Plugins`_), or it
   will be the responsibility of the caller to set the submodule.

The entire implementation of our plugin looks like:

.. literalinclude:: ../../../../tests/cxx/doc_snippets/pluginplay_examples.cpp
   :language: c++
   :lines: 17-

Defaults From Other Plugins
===========================

A plugin can not set a submodule to a module from another plugin in its
constructor, because the other plugin may not be loaded. Instead, the plugin
lists the plugins it would like defaults from, highest priority first, and
records which of their modules it would like to use:

.. code-block:: c++

   PLUGIN_CTOR(MyPlugin) {
       set_name("my_plugin");
       set_preferred_providers("provider_a", "provider_b");
       add_module<MyDriver>("My driver");

       default_submod_from("provider_a", "My driver", "Data", "A data");
       default_submod_from("provider_b", "My driver", "Data", "B data");
   }

When the user calls ``ModuleManager::wire_defaults``, the ``"Data"``
submodule of ``"My driver"`` is set to ``"A data"`` if ``provider_a`` is
loaded, otherwise to ``"B data"`` if ``provider_b`` is loaded. Submodules the
user has already set are never changed.

Python
======

To make your plugin usable from Python, create its Python module with the
``EXPORT_PLUGIN`` macro. The macro takes the name of the Python module, the
name of the plugin class, and the name of the pybind11 module variable. It is
followed by the body of a function which can export anything else the Python
module needs, *e.g.*, property types:

.. literalinclude:: ../../../../tests/python/doc_snippets/export_pluginplay_examples.cpp
   :language: c++
   :lines: 24-39

Python users then load the plugin with:

.. code-block:: python

   import pluginplay as pp
   import pluginplay_examples as ppe

   mm = pp.ModuleManager()
   mm.load_plugin(ppe.PluginplayExamples())
