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

The call graph of ``ModuleManager::run_as`` is summarized in
:numref:`fig_run_as_call_graph`.
