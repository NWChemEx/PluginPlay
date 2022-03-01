###########
Memoization
###########

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
facilitating checkpoint/restart. Very briefly, the idea is that we assume we can
save/load the memoization details to disk. If we load a previous run's
memoization details and then run a previously run module (with an input it has
already seen) the program will use memoization to quickly achieve parity wit its
previous run (*i.e.*, all module calls will be memoized until control finds the
first module which hasn't run yet). The exact design details pertaining to
checkpoint/restart are beyond this section (see
:ref:`_design_checkpoint_restart` for the full discussion), rather our point
here is that in PluginPlay, memoization is coupled to checkpoint/restart and
discussions of either need to consider the other.

**************************
Memoization Considerations
**************************

Considerations Pertaining to When Memoization Is Used
=====================================================

- Ideally the use of memoization is under the hood of PluginPlay and largely
  transparent to the user.
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

Considerations Pertaining to How Memoization is Used
====================================================

- Memoizing iterative functions requires special considerations (vide infra)
- Memoization will need to occur in a parallel setting


- Comparing objects can be expensive (think about distributed tensors)


- Memoization only needs to remain valid on a short time frame (the current run
  or a restart immediately following the run). Long term archival falls to
  checkpoint/restart
- May make sense to build resiliency into the memoization solution


**********************************************
Considerations for Memoizing Iterative Modules
**********************************************

Within PluginPlay it is somewhat natural to write a module which encapsulates
one iteration of an iterative process and to then call the module in a loop.
Let's say ``I`` was the initial input to the module and ``R_0`` was the initial
result, the the repeated calls to an iterative module play out something like:
given input ``I`` return ``R_0``, given ``R_0`` return ``R_1``, given ``R_1``...
In general ``R_i`` is the input to iteration :math:`i+1` and the result of
iteration :math:`i`. In this case attempting to memoize a second loop set of
iterations, given ``I``, would only work if all intermediate results are
memoized. In such case control looks like: Give module ``I`` get back memoized
``R_0``, give module ``R_0`` get back memoized ``R_1``, etc. Particularly for
large results, we may not be able to afford storing all the intermediates.


***************************
Memoization Implementations
***************************

Since memoization will occur under the hood of PluginPlay it is possible to
incorporate existing solutions if they are written in C or C++. A quick search
for C++ Memoization libraries turned up the following options:

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
functions by wrapping them and in manners akin to how the above libraries do so.

.. note::

   The notes on the memoization libraries are based off of documentation and
   quick skimming of the source code. I do not have experience with any of the
   libraries so the descriptions may be inaccurate. Update, star, and watcher
   information was accurate as of March 2022 and may have changed since then.

*******************************
Potential Memoization Solutions
*******************************

With the lack of well supported, feature-rich memoization libraries writing our
own memoization routine seems like the best route. Conceptually the actual
memoization process is straightforward since the ``Module`` class's ``run``
method already wraps the actual invocation of the ``ModuleBase`` class's
``run`` method (the latter is what actually implements the module's algorithm).
The actual memoization process more or less amounts to adding code before the
call to ``ModuleBase::run`` which checks to see if we've seen thes inputs before
(returning the cached results if we have), and code after the call to
``ModuleBase::run`` which caches the results.

For PluginPlay, the complexity associated with memoization comes from storing
and retrieiveng the memoized results. Even if we used one of the existing
memoization libraries we'd still have to tackle these considerations ourselves.
As far as the remaining memoization considerations are concerned:

- Not all modules are memoizable. A module must be "referentially transparent"
  in order to be memoized (always computes the same value for the same inputs,
  and has no side-effects).

  - PluginPlay has no way of knowing what the algorithm inside a module does and
    thus can not determine if the module is referentially transparent or not. As
    a result we must rely on module developers to tell us whether a module
    should be memoized or not. In PluginPlay this is handled by having the
    developer call ``ModuleBase::turn_off_memoization`` in the module's ctor.

- Memoization is a time for space trade-off meaning we save time at the cost of
  storing additional data.

  - For coarse-grained control the ``Module`` class exposes
    ``turn_off_memoization`` which the user can call to disable memoization for
    that module.

- Comparing objects can be expensive (think about distributed tensors)

  - Since every module call can potentially be memoized, the object comparisons
    will likely happen often enough that we need to worry abou this. The
    standard solution here is

- Memoizing iterative functions requires special considerations (vide infra)


- Memoization will need to occur in a parallel setting
- When memoizing it only makes sense to consider results computed by other
  module implementations of the same type.

  - Type here is not "property type", but rather the literal class type which
    derives from ``ModuleBase``

- Memoization only needs to remain valid on a short time frame (the current run
  or a restart immediately following the run). Long term archival falls to
  checkpoint/restart
- May make sense to build resiliency into the memoization solution
