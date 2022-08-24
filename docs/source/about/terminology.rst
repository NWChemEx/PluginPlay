###########
Terminology
###########

The purpose of this page is to introduce the terminology used by PluginPlay .

***********
Basic Terms
***********

module
   a self-contained piece of code. Modules wrap developer provided code
   in a common API that can be used by the rest of the PluginPlay. End-users
   typically do not work directly with modules, they work with...

property type
   A property type is a domain-specific quantity of interest. For
   example if your code computes geometric properties of shapes then you may
   have property types for area, volume, and perimeter. Basically these are the
   quantities that users of your code will want to compute and they define the
   manners in which a module can be called.

plugins
   These are groups of modules that are distributed together.
   In C++ plugins will be libraries, whereas in Python plugins will be
   Python modules.

memoization
   A technique where repeated calls to an expensive function, with
   the same input, are avoided by saving the input to the function and the
   result of calling the function.

fields
   How we generically refer to the arguments to/from a function, *i.e.*, both
   inputs and results are fields.

************
Module State
************

bound input
   For a particular module, ``M``, an input ``k`` is said to be
   bound if the ``ModuleInput`` instance returned by ``M.inputs().at("k")`` has
   a value. When a module is called as a particular property type, all inputs
   not appearing in the definition of the property type must be bound.

bound submodule
   Same as a "bound input" except that it refers to one of the
   submodules that a module will call instead of one of the input options.

transparent
   An input to a module is "transparent" if the result of the
   module does not depend on the value. Most inputs are "opaque" in that their
   values do change the value. Examples of transparent options include debug
   printing levels and options that affect the speed at which an algorithm
   computes (block sizes, number of threads, *etc.*).

opaque
   The opposite of transparent. An option is "opaque" it changing its
   value changes the result of a module.

locked
   A module is said to be locked if the end-user accessible state can no
   longer be changed. This occurs when a module is run and prevents concurrent
   access from invalidating memoization.
