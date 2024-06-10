.. Copyright 2024 NWChemEx Community
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

#####################
How Does run_as work?
#####################

``ModuleManager::run_as`` is the workhorse of running a call graph. The point
of this page is to succinctly summarize the behind-the-scenes actions of this
function.

.. _fig_run_as_call_graph:

.. figure:: assets/fig_run_as_call_graph.png
   :align: center

   Overview of the control flow of ``ModuleManager::run_as``. Boxes denote
   methods, ovals data. Arrows between methods point from caller to the callee.
   Arrows stemming from data indicate data is input. Arrows ending in data
   indicate that data results. When a method calls multiple sub-methods the
   call order is depicted left-to-right.

The call graph of ``ModuleManager::run_as`` (as seen from C++)  is summarized in
:numref:`fig_run_as_call_graph`. Briefly:

#.  Many users enter the call graph via ``ModuleManager::run_as``; however, this
    method simply wraps retrieving the specified module and forwarding the
    results to that module's  ``Module::run_as`` method.
#.  ``Module::run_as`` is the typed user interface. It works with the objects'
    native C++ types, i.e., inputs and results are actual C++ objects. Under the
    hood ``Module::run_as`` is responsible for type-erasing inputs and restoring
    types to results.
#. ``PropertyType::wrap_inputs`` this is the method that ``Module::run_as``
   calls to type-erase the inputs. The property type class is essentially a
   schema written in terms of domain-specific types.
#. With type-erased inputs the next call is to the type-erased user-interface,
   ``Module::run``.
#. The first thing ``Module::run`` does is to merge the user-provided inputs
   with the bound inputs. The result is the full set of inputs for the module.
#. Next, ``Module::run`` determines if the module is ready to run, e.g., does
   it have all the inputs needed to run? Has a submodule been assigned to each
   callback point? Is each submodule ready to run?
#. Assuming the module is ready to run, ``Module::run`` then locks the module
   (and all submodules). A locked module can no longer have its options or
   submodules changed.
#. Next ``Module::run`` runs any "pre-run" hooks. These are operations which
   should be done before actually running the module. At present, this is
   limited to checking for memoization options, but user expansion is possible.
#. After pre-run hooks are run the actual module contents are run via
   ``ModuleBase::run`` (which calls the derived class's ``run_`` method).
#. After control returns to ``Module::run`` from ``ModuleBase::run``,
   ``Module::run`` runs any post-run hooks (by default this includes caching
   results, but again users can expand).
#. The results of ``Module::run`` are type-erased, so the final step is for
   ``Module::run_as`` to restore their types before returning them to the user.
