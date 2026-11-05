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

.. _call_graph_state_design:

######################
State of a Call Graph
######################

This page lists the state that determines what running a module does, and
says which class holds each piece at runtime. It covers the state that exists
today and the state that drivers (see :ref:`driver_design`) add.

.. note::

   Drivers are not implemented yet. ``DriverBase``, ``CallGraphRequest``, and
   ``OverlayPIMPL`` are the planned classes of this page and
   :ref:`driver_design`.

***************************
What is Call Graph State?
***************************

Running a module runs the modules bound to its submodules, which run the
modules bound to theirs, and so on. The state of a call graph is everything
that decides what that run does:

- the *shape* of the graph, *i.e.*, which module is bound to each submodule;
- the *inputs* each module runs with; and
- the *bookkeeping* PluginPlay keeps for each module: whether it is locked,
  whether it memoizes, its cache, its timer, and its runtime.

As :ref:`call_graph_design` established, the nodes of the call graph are
``Module`` objects. A ``Module`` holds its inputs and the submodule requests
that bind its children, so the graph is the set of ``Module`` objects
reachable from the one being run.

******************************
Why Document Call Graph State?
******************************

Drivers set the inputs and submodules of modules below them, without changing
the modules the ``ModuleManager`` holds. That needs somewhere to keep the
values a driver sets. An earlier design added a node class, ``CallGraphNode``,
for this (see `Comparison With CallGraphNode`_). ``Module`` is already the node
of the call graph, and a driver is a module, so this page sets out where each
piece of state lives, and how the state drivers add lives in ``Module`` and its
PIMPL.

*************************
Call Graph State Concerns
*************************

#. **One owner per layer.**

   - Each layer of settings is stored by the class of whoever sets it, so
     setting one layer can not change another.

#. **No copies of state.**

   - A layer that changes a few values points to the layer below it and
     stores only what it changes. Inputs may be large, *e.g.*, a molecule or
     a basis set, so they are not copied to change one setting.

#. **No new classes without need.**

   - ``Module`` is the node of the call graph, for drivers too. A new class is
     added only for state it can not hold.

#. **The ModuleManager's modules are not changed by drivers.**

   - A module the ``ModuleManager`` holds runs the same way on its own after a
     driver has used it.

#. **Sharing is preserved.**

   - A module bound in several places in the call graph is one object, so it
     runs, memoizes, and is timed as one module.

#. **State is immutable once locked.**

   - Every layer a locked module reads is locked too, so nothing a module
     points to can change under it.

#. **Memoization is correct.**

   - Two modules with the same memoization key compute the same results.

**********************
The Layers of Settings
**********************

The settings of a module come in four layers. Each layer is set by someone
different, and each later layer overrides the earlier ones:

.. list-table::
   :header-rows: 1
   :widths: 22 20 28 30

   * - Layer
     - Set by
     - How
     - Stored in
   * - 1. Module defaults
     - Module developer
     - ``add_input``, ``add_submodule``, ``set_default``, in the module's
       constructor
     - ``ModuleBase``
   * - 2. User choices
     - User
     - ``change_input``, ``change_submod``, ``turn_off_memoization``
     - ``ModulePIMPL``, through ``Module``
   * - 3. Driver defaults
     - Driver developer
     - ``set_target`` and ``set_default`` on the driver's ``add_input`` and
       ``add_submodule``, ``when``, and ``wire_memoization``, in the driver's
       constructor
     - The ``CallGraphRequest`` in ``DriverBase``
   * - 4. User overrides of the driver
     - User
     - ``change_input`` and ``change_submod`` on the driver, and
       ``set_input`` and ``set_submod`` on
       ``Module::call_graph_request()``
     - The driver's ``ModulePIMPL``, including its ``CallGraphRequest``,
       through ``Module``

Layers 3 and 4 parallel layers 1 and 2: the developer's part is in the
implementation class, and the user's part is in ``ModulePIMPL``. There are
only two places to store settings, ``ModuleBase`` (or ``DriverBase``) and
``ModulePIMPL``, for modules and drivers alike.

Layers 1 and 2
==============

``ModuleBase`` holds the module developer's declarations: the inputs with
their defaults, the submodule requests with their property types and no
module bound, the property types and results, the description, and the
citations. It also holds the UUID and the runtime the ``ModuleManager`` gives
it, and the module's internal cache. The ``ModuleManager`` keeps one
``ModuleBase`` per module type, and every ``Module`` of that type shares it
through a ``std::shared_ptr``, so a module's implementation is never copied.

``ModulePIMPL`` holds the user's choices: the inputs, the submodule requests
with the modules bound to them, and whether the module memoizes. It also holds
the module's bookkeeping: whether it is locked, the pointer to its cache in
the ``ModuleManager``'s caches, and its timer.

Today ``ModulePIMPL`` does not store layer 2 as changes to layer 1. Its
constructor copies all of the inputs and submodule requests of the
``ModuleBase``, and ``change_input`` changes the copy. The copy is made once,
when the module is made, before any large input is set, so it is cheap.

Layers 3 and 4
==============

A driver is a module, so it has layers 1 and 2 of its own: its inputs and
submodules, declared by the driver developer and changed by the user. Layers 3
and 4 are not values of the driver. They are *wiring requests*: what to set
on the modules below it:

- ``DriverBase`` holds the driver developer's ``CallGraphRequest``: the
  requests made with ``set_target``, the conditional chains, the
  memoization policy, and the default modules for the driver's submodules (see
  :ref:`driver_design`). ``ModuleBase`` returns an empty request, so a module
  that is not a driver has no layer 3.
- The driver's ``ModulePIMPL`` holds the values the user gave the driver's
  inputs and submodules, which the ``set_target`` requests pass on, and the
  user's ``CallGraphRequest``, which holds the fixed values the user set with
  ``set_input`` and ``set_submod``. The user's request is empty for a module
  that is not a driver, and ``Module::call_graph_request()`` throws for one.

The wiring requests name their targets by module key, so they become values
only when the ``ModuleManager`` resolves the keys, which is when the driver is
locked.

******************
Where State Lives
******************

The layers above are what people set. The state a run reads is the result of
combining them. This section says where that combined state lives.

Outside a Driver
================

The state of a module that is not below a driver is fully specified by layers
1 and 2, which are already combined in its ``ModulePIMPL``. No additional state
changes are needed to run the module. The ``Module`` objects are thus the
nodes, and the submodule requests in their PIMPLs are the edges in the call
graph.

Below a Driver
==============

Until a driver is locked, drivers store wiring requests and not the actual
changes. When a driver is locked, the ``ModuleManager`` resolves the requests
and the changes must live somewhere without changing the state of the original
modules (which may still be used by other call graphs). The proposal is that
we introduce a new Module PIMPL that points to the original module and stores
the changes alongside it. This is called an *overlay module*.

- **What it is.** An overlay module is an ordinary ``Module`` whose PIMPL is an
  ``OverlayPIMPL``, which derives from ``ModulePIMPL``. It points to the
  original ``Module``, *i.e.*, the ``ModuleManager``'s module, and stores only
  the inputs and submodule bindings that differ. Reading an input it does not
  store reads the original's. It shares the original's ``ModuleBase`` and
  cache, as every module of that type does. An overlay may point to another
  overlay, so changes can be chained.
- **When one is made.** When a driver is locked, the ``ModuleManager``
  resolves the driver's wiring requests. A module gets an overlay module if a
  request changes one of its inputs or submodules, or if a module below it got
  one, since its submodule must then be bound to the overlay module. Every
  other module is used as is. Only modules in the driver's region, *i.e.*,
  reachable without passing through a nested driver, are considered.
- **Sharing.** The ``ModuleManager`` keeps a map from each original module to
  its overlay module while it resolves a driver, so a module bound in several
  places gets one overlay module. Any number of drivers can share a module
  that none of them changes. Drivers that change it each get their own
  overlay, and overlays with the same settings share results through
  memoization (see `Memoization`_).
- **Who owns them.** The driver does. Its ``ModulePIMPL`` holds the overlays
  and the submodule map the driver runs with, which binds its submodules to
  overlay modules where needed. Both are empty for a module that is not a
  driver. The driver's own submodule requests, which hold the user's
  bindings, are not changed. Copying the driver, *e.g.*, with
  ``ModuleManager::copy_module``, does not copy the overlay modules, so the
  copy resolves its wiring requests again when it is locked.
- **Running.** An overlay module is a ``Module``, so the driver calls it through
  its submodule request like any other, and it runs through the usual
  ``Module::run``. No submodule request needs to know about a separate node.
- **Nested drivers.** A nested driver whose inputs an enclosing driver sets
  gets an overlay module like any other module. When it is locked, its wiring
  requests are resolved with its overlaid inputs, and its overlay modules are
  owned by it. Since the resolved changes live in modules, any call graph that
  reaches a driver reuses its overlays, which is what lets a driver stand for
  its whole subgraph. A module reachable both from an enclosing driver and
  below a nested driver may run with two configurations (see
  :ref:`driver_design`).
- **Locking.** Locking a driver locks the overlay modules and the originals
  they point to, so neither can change while the driver uses them. This is
  today's rule, under which running a module locks the modules below it.

Implementing the Overlay PIMPL
------------------------------

``ModulePIMPL`` is not polymorphic today. ``OverlayPIMPL`` needs it to be,
with a virtual destructor and a virtual ``clone``, so that copying a
``Module`` copies the right kind of PIMPL. Three parts of today's code shape
``OverlayPIMPL``:

#. **The accessors return whole maps.** ``ModulePIMPL::inputs()`` and
   ``ModulePIMPL::submods()`` return references to complete maps, as do
   ``Module::inputs()`` and ``Module::submods()``, and running a module merges
   its inputs with that map. Copying a ``ModuleInput`` copies its value, since
   ``AnyField`` copies what it wraps, so a complete input map built by copying
   would copy every input of the original, including large ones.

   The proposal is to add a virtual ``cref_clone()`` to ``AnyFieldBase``.
   ``AnyFieldWrapper<T>`` can already hold a ``const T&``, which is how
   ``ModuleInput`` stores inputs declared as const references, and each
   wrapper knows its ``T``, so it can make a wrapper of a ``const T&`` to its
   own value. ``OverlayPIMPL`` then builds a complete input map in which each
   input it does not change refers to the original's value, and only the
   inputs it changes hold values of their own. ``Module::inputs()`` keeps its
   signature, and nothing is copied. This is safe because the original is
   locked, so its values can not change, and the overlay keeps it alive.

   The submodule map has no such problem: a submodule request holds a
   ``std::shared_ptr`` to its module, so copying it copies no module.
#. **Overlays of drivers.** The overlay of a driver must still be a driver.
   Being a driver is a property of the implementation, which the overlay
   shares with the original, so ``is_driver()`` and the developer's request
   come for free. The overlay copies the original's user request, which is
   small, so it can resolve the requests itself.
#. **Lifetime and locking.** The overlay holds a ``std::shared_ptr`` to the
   original ``Module``, so the original lives as long as the overlay. Locking
   an overlay locks the original too.

With these, ``OverlayPIMPL`` adds little to ``ModulePIMPL``: the pointer to the
original, and which inputs and submodules it changed. Whether it needs to be a
class of its own, or is a ``ModulePIMPL`` with an optional pointer to an
original, can be decided when it is implemented.

Bookkeeping
===========

.. list-table::
   :header-rows: 1
   :widths: 25 35 40

   * - State
     - Lives in
     - Notes
   * - Lock flag
     - ``ModulePIMPL``
     - Overlay modules are locked when made.
   * - Memoization on/off
     - ``ModulePIMPL``
     - The driver's memoization policy is resolved into the overlay modules.
   * - Module cache
     - ``ModuleManager`` caches, pointed to by ``ModulePIMPL``
     - An overlay module shares its original's cache.
   * - Memoization key
     - Computed by ``ModulePIMPL`` when run
     - Must include the state below the module, see `Memoization`_.
   * - Timer
     - ``ModulePIMPL``
     - An overlay module has its own, so a run below a driver is timed
       separately from the original.
   * - Internal cache, runtime, UUID
     - ``ModuleBase``
     - Shared by every module of the type, overlay or not.
   * - Inactive settings
     - The driver's ``ModulePIMPL``
     - Recorded when the driver's wiring requests are resolved, and returned
       by ``Module::inactive_settings()``.
   * - Fingerprint
     - ``ModulePIMPL``
     - Computed when the module is locked, see `Memoization`_.

Memoization
===========

Today a memoization key is a module's inputs plus the UUIDs of its submodules'
implementations. Modules of the same type share a ``ModuleBase``, and so a
UUID, so two modules with different state below them can have the same key. An
overlay module shares its original's cache, and its children share their
originals' UUIDs, so it would find the original's results. This is not
specific to drivers: modules of the same type with different submodules have
the same problem today.

The key must instead identify the state of everything below the module. Each
module, overlay or not, gets a *fingerprint* when it is locked, computed from:

- the UUID of the module's implementation,
- the module's bound inputs,
- whether the module memoizes, and
- the fingerprints of the modules bound to its submodules, labeled by
  submodule key.

Two modules have the same fingerprint exactly when they and everything below
them would run with the same state. The memoization key of a call is the
merged inputs plus the fingerprints of the submodules, replacing today's
submodule UUIDs. This removes the false matches, and lets an overlay share
results with the original, or with another driver's overlay, when they really
are the same, so drivers need no caches of their own.

The fingerprint must be cheap to compare and to store in many keys, so it is
an interned ID: a ``FingerprintTable`` maps the full description of a module,
*i.e.*, the four items above, to an ID. A fingerprint is a string, so the map
from submodule keys to fingerprints has the type the submodule UUIDs had, and
the cache and its databases are unchanged. Each ``ModuleManager`` owns a table
and gives it to its modules when it attaches them. A module that is not in a
``ModuleManager`` uses a table of its own. The table must not outlive the
``ModuleManager``, since it holds Python objects among the inputs it has seen.
A table shared by the whole process was tried and crashed when it was
destroyed after the Python interpreter.

*****************************
Comparison With CallGraphNode
*****************************

An earlier design, :ref:`call_graph_class_design` before it was cut back,
made the call graph explicit with a ``CallGraphNode`` for every module, and
ran modules as nodes. ``CallGraphNode`` and an overlay module hold the same
thing: a pointer to a module plus the values a driver changed. The differences
are:

- An overlay module is a ``Module``, so no new public class is needed, and the
  usual ``Module`` functions, *e.g.*, ``inputs()``, ``submods()``, and
  ``profile_info()``, inspect it.
- Submodule requests bind to overlay modules as they bind to any module, so
  they need no node pointer, and ``Module::run`` needs no node overload.
- The earlier ``CallGraph`` built a node for every module in the graph, driver
  or not. An overlay module is made only for a module below a driver whose
  state, or the state below it, changes. The cost is that a module above a
  changed module is overlaid too, although only its submodule binding
  differs.

**********************
Further Considerations
**********************

- **Layer 2 as changes.** ``ModulePIMPL`` could store layer 2 as changes to
  layer 1, as an overlay module stores changes to its original. Nothing needs
  this yet.
- **The CallGraph class.** A read-only ``CallGraph``, to inspect what a module
  will run, including the overlays a driver made and the settings that would
  be inactive, is left for later (see :ref:`call_graph_class_design`).
- **Inspection API.** How a user tells an overlay module from an original, and
  sees which values a driver changed, *e.g.*, ``Module::is_overlay()`` and
  ``Module::overrides()``, is left open.
- **Copies in the fingerprint table.** The ``FingerprintTable`` copies the
  inputs of each description it stores. It could hold ``cref_clone`` copies
  instead, but only if the modules they refer to outlive the table, which an
  overlay of a copied or destroyed driver does not. This is left for later.
- **Python.** Overlay modules are ``Module`` objects, so the Python bindings of
  ``Module`` cover them.
