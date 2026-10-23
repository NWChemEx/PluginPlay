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

.. note::

   This class has not been designed yet. This page records what it is for, so
   that the pages that refer to it have something to point at.

*********************
What is a CallGraph?
*********************

A ``CallGraph`` is a read-only view of everything a module will run when it is
called: the module, the modules bound to its submodules, the modules bound to
their submodules, and so on. Its nodes are the ``Module`` objects themselves,
including the overlay modules a driver makes (see
:ref:`call_graph_state_design`). It holds no state of its own that affects a
run. Modules run without it.

***************************
Why Do We Need a CallGraph?
***************************

Today the call graph is implicit: each ``Module`` holds the modules bound to
its submodules. That is enough to run a module, but not to answer questions
about the whole graph:

- What will this module run, and which of those modules did a driver change?
- Which of a driver's settings would have no effect, before the driver is
  locked and a calculation is paid for?
- Where was time spent in the last run?

The ``CallGraph`` answers them. It would also be the natural place for other
operations on the whole graph, *e.g.*, finding a module by key, printing the
graph, or recording it with a calculation's results for provenance.

**********************
Further Considerations
**********************

- **History.** An earlier version of this page designed a ``CallGraph`` that
  was what ran: every module ran as a ``CallGraphNode``, which pointed to its
  module and held the values a driver changed. It was replaced by overlay
  modules, which hold the same values in a ``Module``, so no node class is
  needed and modules run as before. :ref:`call_graph_state_design` compares
  the two.
- **Considerations to carry over.** The view should be cheap to make, since it
  only refers to modules, and it must not change any module. A view of a module
  that is not locked should show what locking would do, without locking
  anything.
