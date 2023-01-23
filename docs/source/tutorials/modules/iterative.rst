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

*****************
Iterative Modules
*****************

Modules that are meant to be called as part of a loop (typically when optimizing
a quantity) require special consideration. Consider a module that takes an input
:math:`i_0` and returns a result :math:`r_1`. If :math:`i` depends on :math:`r`
we can then compute a new :math:`i`, :math:`i_1` using :math:`r_1`, and call the
module again generating :math:`r_2`. This can be repeated until convergence.
There's two main considerations for an iterative module: intermediate quantities
and memoization.

Many of the more sophisticated algorithms for iterative refinement, in one way
or another, use information from previous iterations to help accelerate
convergence. It is essential to the algorithm to be able to access the
information from the previous iteration. Memoization leads to a similar
difficulty. Say our iterative process ran :math:`m` times, so we have computed
an :math:`r_1`, an :math:`r_2`, on up to an :math:`r_m`. When the calculation
restarts we want to jump back to :math:`r_m`, but the input to the module will
be :math:`i_0` and thus default memoization will provide us :math:`r_1`. In
theory if we stored all of the intermediate :math:`r` values: :math:`r_2`,
:math:`r_3`, etc. up to :math:`r_m`, we quickly would regain parity with the
original computation. In practice we usually delete the intermediates to save
memory meaning we no longer have access to them. While it is possible to solve
the memoization problem by recasting iteration as recursion, it is not natural
in many circumstances and we wish to avoid it.

PluginPlay's solution to this problem is to allow each module to cache
intermediate results. Here we mean "intermediate" in the sense that they are
either not the result the module will return or they are not the converged
result. Assuming that the property type for the iterative module is defined such
that it always takes the intial inputs, in addition to the current inputs, one
can then save the intermediate results in the cache unsing the initial value as
the key.

As an example we show how to write a simple module for determining a root of the
cosine function given an interval around the root:

.. literalinclude:: ../../../../tests/pluginplay/docs/module_cache.cpp
   :language: c++
