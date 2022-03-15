.. _memoization_design:

##################
Memoization Design
##################

This page documents the design process and decisions affecting memoization in
PluginPlay.

********************
What is Memoization?
********************

Memoization is a run-time optimization technique used to speed up a program by
storing the inputs and results of function calls (cacheing). When a function is
called with already cached inputs, the cached results are returned without
actually recomputing the function. Memoization thus represents a time-for-space
trade off.

*************************************
Why does PluginPlay Need Memoization?
*************************************

PluginPlay relies on rigid property type APIs to define what data is directly
passed to and from modules. Inevitably, users will have modules for which these
APIs fail to provide all of the necessary inputs. PluginPlay's solution to this
is to allow module developers to call submodules. In this scenario, the module
developer gets the additional input by calling the submodule. Depending on what
this additional data is, this solution can be expensive if the submodule is
used to recompute the same value multiple times. Memoization ensures that the
submodule is only run once for each unique input.

Memoization was originally added to PluginPlay to avoid expensive recomputation;
however, it was later realized that memoization could play a dual role by also
facilitating checkpoint/restart (C/R). Very briefly, the idea is that we assume
we can save/load the memoization details to disk. If we load a previous run's
memoization details and then run a previously run module (with an input it has
already seen) the program will use memoization to quickly achieve parity with
its previous run (*i.e.*, all module calls will be memoized until control finds
the first module which hasn't run yet). In fact, it is difficult to concieve of
a C/R strategy that doesn't use memoization, as at its core C/R is a type of
memoization (one is ultimately avoiding recomputing something by reading it
from the checkpoint). The exact design details pertaining to C/R are beyond this
section (see :ref:`design_checkpoint_restart` for the full discussion), rather
our point here is that in PluginPlay, we consider C/R part of the overall
memoization strategy.

**************************
Memoization Considerations
**************************

The following list contains considerations related to either how the process of
memoization is implemented in PluginPlay or how the memoized data is used.

- Ideally the use of memoization is under the hood of PluginPlay and largely
  hidden from the user.
- Assuming automation, PluginPlay needs to know whether a module is memoizable
  or not.

  - Not all modules are memoizable. A module must be "referentially transparent"
    in order to be memoized (always computes the same value for the same inputs,
    and has no side-effects).

- Memoization is a time for space trade-off meaning cost of storing data will
  play a role in whether memoization can/should occur.

  - User will likely need some control, automation is unlikely to cover all use
    cases.
  - Since memoization occurs when calling modules, letting the user manually
    turn off memoization for select modules provides them full control.

- Each class ``T`` deriving from ``ModuleBase`` is assumed to wrap a unique
  algorithm. When memoizing it thus only makes sense to consider results
  computed by other ``T`` instances.

  - ``T`` here is not a "property type", but rather the literal class type which
    derives from ``ModuleBase``
  - The uniqueness of ``T`` is guaranteed by the one-definition rule.
  - In theory users could write exactly the same algorithm in two different
    derived classes. In practice, this is likely to be an extreme edge case that
    is not worth considering. Failing to consider it simply means we miss a
    memoization opportunity.

- Failing to memoize when appropriate will affect performance
- Memoizing when not appropriate will compromise the integrity of the scientific
  answer.
- Memoizing iterative (recursive) function requires special considerations

  - Memoizing an iterative (recursive) function is possible
  - Say ``I`` is the initial input to an iterative function ``f``, and ``R_0``
    is the initial result. Calling ``f`` with ``R_0`` generates ``R_1``, calling
    ``f`` again with ``R_1`` generates ``R_2`` etc. If we want to jump from
    ``I`` to the last result (say ``R_n``) we need to store the ``n`` proceeding
    function calls in addition to the call which generates ``R_n``.
  - For recursion the situation is similar except that ``n`` tracks depth
    instead of iteration.
  - For memory intensive results storing all of the intermediates is expensive
    and we will need an option to avoid it.

- Memoization will need to occur in a parallel setting

  - Will need to memoize parallel objects, which means knowing that not only
    that the local input is the same, but also that the rest of the object is
    the same.
  - Will need to worry about data races to get memoized data.

- Comparing objects can be expensive (think about distributed tensors)

  - Hashing is a possible solution, but relying on hashing alone can
    theoretically lead to memoizing when not appropriate (although if
    implemented correctly the odds are astronomical)

- Need to decide how long memoized data is valid for.

  - Requiring memoized data to be valid long term means that memoization can
    also directly be used for checkpoint/restart
  - Assuming memoized data need only be valid short term, checkpoint/restart
    becomes a separate issue.

- May make sense to build resiliency into the memoization solution

  - Hardware failures could lead to pieces of a distributed object being non
    recoverable and preventing memoization of the object.

***************************
Memoization Implementations
***************************

Since memoization will occur under the hood of PluginPlay it is possible to
incorporate existing memoization solutions into PluginPlay if they are written
in C or C++. A quick search for C++ Memoization libraries turned up the
following options:

- memo

  - https://github.com/jimporter/memo
  - Provides a metafunction ``memoize`` which takes a function and returns a
    wrapped function object. Calls to the wrapped function object (which has
    the same API as the original function) get memoized.
  - The implementation is quite simple and stores copies of the inputs and
    results in a ``std::map``
  - Appears to still be maintained (although the author likely views it as
    feature complete). 16 stars and 2 watchers.

- MemoizationLibCpp

  - https://github.com/guhetier/MemoizationLibCpp
  - Provides a class `Memo` which wraps a function.
  - Uses a hash table under the hood. Type of the hash table can be customized.
  - Likely abandoned. Last update was February 2016. 0 stars and 1 watcher.

- memoizer

  - https://github.com/nordlow/memoizer
  - Actually an executable that can be used to memoize system calls made by the
    wrapped process.
  - Does not memoize C++ calls like we need.
  - Likely abandoned. Last update was March 2017. 1 star and 3 watchers.

The search also returned a number of StackOverflow and blog posts pertaining to
memoization in C++. Generally speaking these posts all detail how to memoize
functions by wrapping them using techniques akin to how the above libraries
perform memoizations. Most of our considerations seem to be out of scope for
the resources I found.

.. note::

   The notes on the memoization libraries are based off of documentation and
   quick skimming of the source code. I do not have experience with any of the
   libraries so the descriptions may be inaccurate. Update, star, and watcher
   information was accurate as of March 2022 and may have changed since then.

********************
Memoization Strategy
********************

With the lack of well supported, feature-rich memoization libraries writing our
own memoization routine seems like the best route. Conceptually the actual
memoization process is straightforward since the ``Module`` class's ``run``
method already wraps the actual invocation of the ``ModuleBase`` class's
``run`` method (the latter is what actually implements the module's algorithm).
The actual memoization process more or less amounts to the following pseudocode:

.. code-block:: c++

   if(this->is_memoizable() && has_result(inputs))
       return get_memoized_result(inputs);

   auto result = this->ModuleBase::run(inputs);

   add_memoized_result(inputs, result);

   return get_memoized_resutl(inputs);


In the first line we rely on the ``Module::is_memoizable()`` method to determine
if the user and the module developer have okayed memoization, if they have we
somehow figure out if we've already seen this result (in the pseudocode this is
the job of the opaque ``has_result()`` function). Assuming the first line is
true we somehow map the inputs to the returned value (in the pseudocode this is
the job of the opaque ``get_memoized_result`` function). If memoization fails,
we run the module's algorithm, store the results (the job of the otherwise
opaque ``add_memoized_result`` function), and then return the memoized results.

This pseudocode addresses the following considerations explicitly and punts the
remaining considerations to the opaque functions:

- Ideally the use of memoization is under the hood of PluginPlay and largely
  hidden from the user.

  - The above pseudocode is in ``Module::run`` and hidden from the user.

- Assuming automation, PluginPlay needs to know whether a module is memoizable
  or not.

  - Handled by ``Module::is_memoizable``.

- Memoization is a time for space trade-off meaning cost of storing data will
  play a role in whether memoization can/should occur.

  - Technically not shown explicitly, but users can use
    ``Module::turn_off_memoization`` to disable memoizing a specific module.
    Calling ``Module::turn_off_memoization`` impacts the value returned by
    ``Module::is_memoizable``.


The opaque functions are consistent with the API of an associative container,
hence the remainder of our memoization strategy is to implement an associative
container which additionally addresses the remaining concerns. We term this
container the ``Cache`` and the design of which can be found at
:ref:`cache_design`.

**************************
Memoization Implementation
**************************

The actual implementation of memoization requires:

- Adding ``turn_off_memoization``, ``turn_on_memoization``, and
  ``is_memoizable`` functions to ``Module`` and ``ModuleBase`` (both also need
  a member to track whether the instance is memoizable)
- Writing the ``Cache`` class
- Ensuring the ``Cache`` instance is accessible through the ``ModuleManager``
- Ensuring the cache is accessible inside ``Module::run``
