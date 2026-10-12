.. Copyright 2026 NWChemEx-Project
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

.. _pp_initialization_design:

##############################
Initializing the ModuleManager
##############################

This page describes how a ``ModuleManager`` goes from empty to holding a
wired call graph built from a set of plugins. Plugins themselves are
described in :ref:`plugin_design`. The ``ModuleManager`` is described in
:ref:`module_manager_design`.

*************************************
What is ModuleManager Initialization?
*************************************

Initialization is the process that:

#. loads the modules from each plugin the user asks for, and
#. sets default submodules between plugins, *e.g.*, makes SCF's driver use
   NUX's Born-Oppenheimer Hamiltonian module when NUX is loaded.

The ``ModuleManager`` is then ready for the user to change it further and
run it.

*************************************
Previous Approaches to Initialization
*************************************

Originally, the end user managed initialization by hand. There was a convention
that plugins would declare/define ``load_modules`` and ``set_defaults``
functions. The end user would call them. This approach has several problems:

- Wiring only works if every provider has been loaded before any
  ``set_defaults`` is called. Nothing enforces that order.
- Every package that combines plugins has to rewrite this sequence.
- PluginPlay cannot tell which plugin a module came from or which plugins
  are loaded.

To try to add some level of automation, we added a function
``ModuleManager::set_default<PT>``. The idea was that a user could call it to
set a default module for a property type. ``ModuleManagerPIMPL::at``
then quietly filled unready submodules of that type whenever a module is
looked up. This feature had no users outside PluginPlay's own tests, and was
removed. The problem was that property types are not unique enough for a single
default to make sense.

*****************************
Initialization Considerations
*****************************

#. Loading order does not matter

   - Plugins can be loaded in any order.

#. Wiring happens after loading

   - "Is my preferred provider loaded?" only has a reliable answer once all
     plugins are loaded. Wiring is therefore a separate step that runs
     afterwards.

#. User choices win

   - Between loading and wiring, the user may change modules, *e.g.*, pick
     submodules or add modules of their own. Wiring must not overwrite those
     choices.

#. Unique names

   - Two plugins may use the same module key, *e.g.*, ``"Overlap"``. Every
     module must still be addressable without ambiguity.

#. Wiring can be run again

   - Running the wiring step again, *e.g.*, after loading another plugin,
     only fills submodules that are still unset.

#. No early errors

   - A submodule that is still unset after wiring is not an error. The user
     can set it at any time before running the call graph.

***********************
Initialization Strategy
***********************

Storage
=======

The ``ModuleManager`` owns each loaded plugin object, and each plugin owns
its ``ModuleMap`` (see :ref:`plugin_design`). So the ``ModuleManager`` has
one ``ModuleMap`` per loaded plugin, plus a ``ModuleMap`` of its own. The
``ModuleManager``'s own map holds modules that do not come from a plugin,
such as modules added directly by the user, lambda modules, and facade
modules. The maps are **not** merged into one pool. A submodule may still be
any module in any map, so links can cross maps.

The empty plugin name, ``""``, is reserved for the ``ModuleManager``'s own
map.

Uniqueness and Lookup
=====================

- ``ModuleMap`` enforces unique module keys within a plugin.
- The ``ModuleManager`` enforces unique plugin names. Loading a second plugin
  with the same name throws.

Together these guarantee that the pair (plugin name, module key) is unique.
The ``ModuleManager`` therefore offers two ways to look up a module, and
every function that takes a module key (``at``, ``change_input``,
``change_submod``, ``run_as``, and so on) supports both:

#. With a plugin name and a module key. This always works. The ``""`` plugin
   name means the ``ModuleManager``'s own map.
#. With only a module key. This works as long as the key appears in exactly
   one map. If it appears in more than one map the call throws and the error
   lists the plugins that contain the key. To support this, the
   ``ModuleManager`` keeps track of which keys appear in more than one map.

The key-only form keeps current code working, since today's module keys are
effectively unique across the NWChemEx plugins.

Loading Plugins
===============

.. code-block:: c++

   pluginplay::ModuleManager mm;
   mm.load_plugin<scf::scf>();
   mm.load_plugin<integrals::integrals>();
   mm.load_plugin<nux::nux>();
   mm.load_plugin<chemcache::chemcache>();

``load_plugin<T>()``:

#. creates a ``T`` (``T`` must derive from ``Plugin`` and be default
   constructible). ``T``'s constructor sets the plugin's name and preferred
   providers and builds its ``ModuleMap``, including its internal wiring,
#. throws if a plugin with the same ``name()`` is already loaded,
#. takes ownership of the plugin object. The plugin's ``ModuleMap`` is now
   the ``ModuleManager``'s map for that plugin; nothing is copied, and
#. updates the record of which keys appear in more than one map.

Python has an equivalent, ``mm.load_plugin(scf.scf())``, which takes an
instance because Python has no template parameters. Because the plugin holds
live modules, the same instance must not be loaded into two
``ModuleManager`` objects.

Wiring Defaults
===============

After loading, the user may make changes, and then calls:

.. code-block:: c++

   mm.wire_defaults();

It iterates over the loaded plugins in the order they were loaded, and for each
plugin ``P``:

#. For each name ``N`` in ``P.preferred_providers()``, in order:

   - If no plugin named ``N`` is loaded, skip it.
   - Otherwise call ``P.default_submods_from(N)``.
   - The result is a list of defaults, each of which is a triple
     (``module_key``, ``callback_key``, ``provider_key``)

#. For each default triple:

   - If ``P``'s module ``module_key`` has  a submodule with key
     ``callback_key`` that is already set, move to the next triple.
   - Else, if the provider does not have a module with key ``provider_key``,
     move to the next triple.
   - Otherwise set ``callback_key`` to the provider's module ``provider_key``.

These changes are done to ``P``'s live map, so it includes any changes the user
made and any wiring done by earlier calls in the loop. Because wiring only
sets unset submodules:

- choices the user made before ``wire_defaults`` are kept,
- the order of ``preferred_providers`` sets priority, since the first
  provider to set a submodule wins, and
- calling ``wire_defaults`` again does not change submodules that are
  already set.

The order in which plugins are loaded does not change the result. Each
plugin only changes its own map, so the result for ``P`` depends only on its
own provider list.
