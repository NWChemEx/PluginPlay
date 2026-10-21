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

.. _call_graph_class_design:

##############################
Designing the CallGraph Class
##############################

This page describes the design of the ``CallGraph`` class, an object that
represents the call graph of one module. :ref:`call_graph_design` describes the
call graph *component*: how modules call each other through property types,
and how the call graph is assembled by binding modules to submodules. That
page ends by noting that a full-fledged graph object could be created later.
This page designs that object.

*********************
What is a CallGraph?
*********************

A ``CallGraph`` is an immutable object describing everything a module will run
when it is called: the module, the modules bound to its submodules, the
modules bound to their submodules, and so on. Each node of the graph refers to
one ``Module`` and records the state that module runs with *in this graph*.
Each edge goes from a module to the module bound to one of its submodules, and
is labeled with the submodule's key.

The state a node runs with is the module's own state, its bound inputs,
submodules, and memoization setting, plus a set of *overrides*. An override
changes that state for this graph only. Overrides are how a driver configures
the modules below it without changing them (see :ref:`driver_design`). A node
without overrides runs exactly as its module would on its own.

***************************
Why Do We Need a CallGraph?
***************************

Today the call graph is implicit. Each ``Module`` holds the modules bound to
its submodules, and running a module walks those links. This works until a
module needs to run *differently* from how the ``ModuleManager`` has it
configured, which is exactly what a driver needs.

An early driver design made the call graph explicit only while a
driver was locked. It copied every module below the driver, applied the
driver's settings to the copies, and rebound the driver's submodules to the
copies. That worked, but it had several problems:

#. **Copies are expensive.** Copying a module copies its bound inputs, which
   may be large, *e.g.*, a molecule or a basis set, and the driver copied every
   module below it whether or not it changed anything about it.
#. **The driver needed the ModuleManager.** Settings name modules by their keys
   in the ``ModuleManager``, so every driver held a handle back to the
   ``ModuleManager`` that had to be reset if the ``ModuleManager`` was
   destroyed first.
#. **The driver changed itself.** Locking a driver swapped its submodules for
   the copies, and unlocking (*e.g.*, in ``unlocked_copy``) swapped them back.
#. **Nested drivers needed bookkeeping.** A nested driver had to know which of
   the modules below it were already copies made by an enclosing driver, so it
   would not copy them again.
#. **Memoization was wrong for copies.** A memoization key is a module's inputs
   plus the UUIDs of its submodules' *implementations*. The ``ModuleManager``
   shares one implementation among the modules of the same type, and a copy
   shares its implementation with the original. Two modules with different
   state below them can therefore have the same key. The early implementation
   worked around this by giving every copy a cache of its own, which also
   prevented legitimate reuse.

The last problem is not specific to drivers. Any two modules of the same type
whose submodules differ only in their inputs or in what they call have the
same memoization key today.

An explicit call graph solves these problems: nothing is copied, the
``ModuleManager`` builds the graph so the driver never needs to find modules
itself, building a graph does not change any module, nested drivers are a
matter of the order in which settings are applied, and each node can have a
memoization key that reflects everything below it.

************************
CallGraph Considerations
************************

#. **Existing code keeps working.**

   - ``Module::run`` and ``Module::run_as`` keep their signatures and behavior
     for modules that are not drivers.
   - Module authors keep calling ``submods.at("key").run_as<PT>(...)`` in
     ``run_``.
   - ``ModuleManager::run_as`` keeps its signature.

#. **Shared state, not copies.**

   - A node refers to its module. Building a graph copies no module.
   - The state of a node is the module's state plus overrides, so a node that
     a driver does not touch costs only the node itself.

#. **One node per module.**

   - A module bound in several places in the call graph has one node, so
     sharing is preserved, as it is today.

#. **Immutable once built.**

   - A built graph never changes, so it can be shared, cached, and inspected
     safely.
   - Since nodes read their modules' state, the modules must not change while
     the graph is in use. Locking guarantees this (see `Locking`_).

#. **The ModuleManager wires drivers.**

   - Finding a module by key is the ``ModuleManager``'s job. The
     ``ModuleManager`` builds the graphs of drivers and applies their settings.

#. **Correct memoization.**

   - Two nodes with the same memoization key must compute the same results.
   - Two nodes whose state, and the state of everything below them, is the
     same should be able to share results.

#. **Inspection.**

   - Users can see what a module will run, including what a driver
     configured, before and after running it.
   - Information produced while building or running the graph, *e.g.*,
     inactive driver settings and profiling information, belongs to the graph.

#. **Python.**

   - The graph can be inspected from Python. Python modules and drivers run
     through the same graph as C++ ones.

***********************
Alternatives Considered
***********************

Private copies
==============

This is the early implementation of drivers, described in
`Why Do We Need a CallGraph?`_. It is replaced by this design.

Build the graph in ``ModuleManager::run_as`` only
=================================================

The ``ModuleManager`` would build a graph for every module it runs, and
``Module::run`` would stay as it is.

- ``mm.at(key).run_as(...)`` is a common pattern, and the Python
  ``ModuleManager.run_as`` is implemented with it. On a driver this would run
  the driver without its settings, silently giving different results.
- Submodules are run by ``Module::run`` from inside ``run_``, so a driver used
  as a submodule would never be configured.

The design below instead puts the graph inside ``Module::run``, and has the
``ModuleManager`` call the same path.

Rebuild the graph on every call
===============================

Every call of ``Module::run``, including the calls of submodules from inside
``run_``, would build the graph of the module being run.

- A module called in a loop, *e.g.*, a Fock builder called once per SCF
  iteration, would rebuild its graph every iteration.
- A submodule's graph built from its own state would not contain the
  overrides a driver put on its node.

The design below builds the graph once, at the top, and hands each submodule
its node.

******************
CallGraph Strategy
******************

The design adds:

- the ``CallGraph`` and ``CallGraphNode`` classes;
- a non-public overload of ``Module::run`` that runs a module as a node of a
  graph;
- a node pointer in ``SubmoduleRequest``, so that submodules run as nodes of
  the same graph;
- ``ModuleManager::call_graph(key)``, which builds, and returns, the graph of a
  module.

The Classes
===========

A sketch of the public, read-only API:

.. code-block:: c++

   class CallGraphNode {
   public:
       // The module this node refers to
       const Module& module() const;

       // The state this node runs with: the module's plus the overrides
       const type::input_map& inputs() const;
       const CallGraphNode& submod(const type::key& callback_key) const;
       bool is_memoizable() const;

       // Only the overrides, e.g., what a driver changed
       const type::input_map& input_overrides() const;
       std::set<type::key> submod_overrides() const;

       // Identifies the node's state and everything below it
       const fingerprint_type& fingerprint() const;
   };

   class CallGraph {
   public:
       const CallGraphNode& root() const;

       // Every node reachable from the root, each once
       std::vector<const CallGraphNode*> nodes() const;

       // Driver settings that had no effect when the graph was built
       const std::vector<std::string>& inactive_settings() const;

       // Timing of the last run, per node
       std::string profile_info() const;
   };

A ``CallGraph`` owns its nodes through ``std::shared_ptr<const
CallGraphNode>``. A node owns its children the same way, and holds a
``std::shared_ptr`` to its module, so the modules outlive any graph that refers
to them. Since nodes are immutable, two graphs may share nodes.

Overrides are stored as a delta: a node stores only the inputs that differ
from its module's, and the submodules it rebinds. ``inputs()`` combines the
module's inputs with the overrides when the node is built, so a node without
overrides stores nothing extra.

Building a Graph
================

A graph is built in two steps.

#. **Compose.** The graph of a module is a node for the module whose
   children are the graphs of the modules bound to its submodules. Composing
   is done with a map from each ``Module`` to its node, so a module reached
   along several paths gets one node. A locked module already has a graph (see
   `Locking`_), which is reused as is rather than recomposed.
#. **Apply driver settings.** For each driver node, starting at the root and
   going down, the driver's settings are applied to the nodes below it (see
   `Applying Driver Settings`_).

Composing needs only the modules' own state. Applying driver settings needs to
find modules by key, and so needs the ``ModuleManager``. A graph can therefore
be built in two ways:

- ``ModuleManager::call_graph(key)`` does both steps.
- A module that is run outside of the ``ModuleManager`` composes its own graph.
  If any node of that graph is a driver, building it throws (see
  `Running a Graph`_).

Applying Driver Settings
------------------------

The driver design doc describes the settings and their precedence (see
:ref:`driver_design`). In graph terms, for a driver node ``D``:

#. **Region.** The *region* of ``D`` is the set of nodes reachable from ``D``
   without passing through another driver node. The nested driver nodes
   themselves are in the region, so that ``D`` can set their inputs, but the
   nodes below them are not.
#. **Targets.** A setting names its target module by key. The
   ``ModuleManager`` finds the module, and the target is that module's node.
   If the module has no node in the graph, the setting is inactive. If the node
   is not in ``D``'s region, *i.e.*, it is below a nested driver, building the
   graph throws. An unknown key also throws, except for a qualified key whose
   plugin is not loaded, which is inactive.
#. **Overrides.** ``wire_input`` and forwarded inputs become input overrides on
   the target node. ``wire_submod`` and forwarded submodules become submodule
   overrides. A module that ``wire_submod`` names, but that is not yet in the
   graph, is composed into the graph first. The memoization policy becomes
   memoization overrides.
#. **Order.** Settings are applied in the driver's precedence order, and the
   last write to a target wins. Forwarded values are read from ``D``'s node,
   so a value an enclosing driver forwarded into ``D`` is the one ``D``
   forwards.

Nested drivers are handled by the order of the outer loop: an enclosing
driver's settings are applied before those of the drivers in its region. No
other bookkeeping is needed.

Building is the only time nodes are modified. Nodes are made immutable when
building finishes.

Changing a node that is shared with another graph, *e.g.*, a node of a locked
submodule's graph, must not change that graph. Applying an override therefore
replaces the node with a modified copy of the node, and replaces each of its
ancestors in this graph so they point to the new node. Only nodes are copied,
never modules.

.. note::

   Whether a setting is inactive is decided when the setting is applied. A
   later setting can rebind the submodule that made a module reachable, so a
   node a setting changed may end up unreachable. A follow-up may instead
   decide inactivity from the finished graph (see
   `Further Considerations`_).

Running a Graph
===============

``Module`` gets a non-public overload that runs the module as a node:

.. code-block:: c++

   class Module {
   public:
       // Unchanged
       type::result_map run(type::input_map inputs);

   private:
       // Runs *this as the node node, which refers to *this
       type::result_map run_(const CallGraphNode& node,
                             type::input_map inputs);

       friend class ModuleManager;
       friend class SubmoduleRequest;
   };

``run_(node, inputs)`` does what ``run`` does today, but with the node's state:

#. merge ``inputs`` with ``node.inputs()`` and check that the node is ready;
#. look up the result in the module's cache, keyed by the merged inputs and
   the fingerprints of the node's children (see `Memoization`_);
#. if it is not there, call the implementation's ``run_`` with the merged
   inputs and a submodule map whose requests point to the node's children; and
#. time the call on the node, rather than on the module.

``SubmoduleRequest`` holds a pointer to the node of the module it is bound to,
null by default. ``SubmoduleRequest::run_as`` calls ``run_`` with that node if
it is set, and ``run`` otherwise. The requests that ``run_`` hands to the
implementation have it set, so every submodule call inside ``run_`` runs as a
node of the same graph, and the graph is built only once, at the top. Requests
made outside a graph, *e.g.*, in a test that calls ``ModuleBase::run``
directly, have it unset and keep today's behavior.

``Module::run(inputs)`` gets the module's graph and runs its root:

- If the module is locked, it already has a graph.
- Otherwise it composes its own graph, which throws if a node is a driver
  (see `Building a Graph`_), then locks and stores it.

``ModuleManager::run_as(key, ...)`` does the same, except that it builds the
graph with ``call_graph(key)``, so it works for drivers too.

The result is that ``Module::run`` keeps today's behavior for every module
that is not a driver, and running a driver with ``Module::run`` instead of the
``ModuleManager`` is an error. No drivers existed before this design, so this
breaks no existing code.

The overload is non-public for now. A public version would have to check that
the node it is given refers to ``*this``, and nothing outside PluginPlay needs
it yet.

Locking
=======

Nodes read their modules' state, so the modules in a graph must not change
while the graph is in use. Running a module therefore locks every module that
has a node in its graph, including modules a driver wired in with
``wire_submod``. This extends today's rule, under which running a module locks
the modules bound to its submodules.

The graph is built when the module is locked, and is stored on the module.
Since a locked module and everything below it can not change, the stored graph
stays valid, and later calls reuse it. ``unlocked_copy``, and therefore
``ModuleManager::copy_module``, does not copy the stored graph. The copy builds
its own graph when it is locked.

.. note::

   Removing locking would require nodes to snapshot the state they read,
   rather than share it. This is left for a later design.

Memoization
===========

Each node has a *fingerprint* that identifies its state and the state of
everything below it. It is computed when the node is made from:

- the UUID of the module's implementation,
- the node's bound inputs, including overrides,
- the node's memoization setting, and
- the fingerprints of its children, labeled by submodule key.

Two nodes have the same fingerprint exactly when they and everything below
them would run with the same state. The memoization key of a call is the merged
inputs plus the children's fingerprints, replacing today's submodule UUIDs.
This removes the false matches described in `Why Do We Need a CallGraph?`_
and lets a driver's nodes share results with the ``ModuleManager``'s modules
when they really are the same, so drivers no longer need caches of their own.

The fingerprint must be cheap to compare and to store in many keys. Storing
the bound inputs of every descendant in every key would not be. The proposed
representation is an interned ID: the ``ModuleManager`` keeps a table from the
full description of a node, *i.e.*, the four items above, to a small ID, and
the fingerprint is that ID. Comparison is then exact and cheap, and the full
description is stored once.

Caches stay on the modules, keyed as above. Moving them to the graph is left
for later (see `Further Considerations`_).

Inspection
==========

``ModuleManager::call_graph(key)`` returns the graph of a module:

- For a locked module, the stored graph. Its ``inactive_settings()`` are the
  settings that had no effect when it was built, and its ``profile_info()``
  is from the last run.
- For an unlocked module, a graph built as locking would build it. It is not
  stored and nothing is locked, so this is a dry run. Building it throws the
  errors that locking would throw.

This replaces the inspection functions the early implementation put on the
``Driver`` class. ``Module::submods()`` keeps showing the ``ModuleManager``'s
bindings, not what a driver configured. What a driver configured is visible
through the nodes' overrides.

Python
======

``CallGraph`` and ``CallGraphNode`` are exported read-only. The Python
``ModuleManager.run_as`` is reimplemented to call the C++
``ModuleManager::run_as`` path rather than ``at(key).run_as``, so that it works
for drivers.

**********************
Further Considerations
**********************

- **Caches on the graph.** With fingerprints, a cache could be keyed by
  fingerprint and owned by the ``ModuleManager`` or the graph instead of by
  each module. That would remove the per-module caches entirely.
- **Removing locking.** See `Locking`_.
- **Inactivity from the finished graph.** Deciding inactive settings after all
  settings are applied, from what is reachable from the root, would also catch
  settings whose target is cut off by a later setting.
- **A public graph overload.** If code outside PluginPlay needs to run a
  module as a node, the overload can be made public with a check that the node
  refers to the module.
- **Persistent caches.** Memoization keys change from submodule UUIDs to
  fingerprints. Results saved with the old keys will not be found by the new
  ones.
- **Graph-wide operations.** An explicit graph makes operations on the whole
  graph possible, *e.g.*, printing it, finding a module by key, or exporting
  it with a calculation's results for provenance.
