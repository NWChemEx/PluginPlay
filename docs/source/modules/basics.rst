**************************
Basics of Writing a Module
**************************

.. |e_field| replace:: :math:`\vec{E}\left(\vec{r}\right)`

This page is intended to walk you through the basics of writing a module. Here
we focus purely on writing the module, not designing it. Because of how the SDE
works designing a module can sometimes be non-trivial; tips for designing a 
module can be found (TODO: add link). This page assumes we are writing a module
which computes the electric field, |e_field| of a series of point charges as 
detailed (TODO: add link).

Delcaring the Module
====================

In the SDE modules are implemented by inheriting from the ``ModuleBase`` class
and then implementing a constructor and a ``run_`` member. The declaration
amounts to a large amount of boilerplate so the SDE provides a macro
``DECLARE_MODULE(T)`` which will declare a class ``T`` satisfying the required
API. Since it will be used as the name of the class, ``T`` must be a valid C 
identifier. 

Generally speaking modules of the same property type differ by the algorithm 
used to compute the property. It thus makes sense to somehow include the 
algorithm in the module's name. For the purposes of our electric field module we
will compute the electric field via Coulomb's Law so we choose to name our
module ``CoulombsLaw`` and we store the declaration in a file ``modules.hpp``.
The relevant lines are:

.. literalinclude:: ../../../tests/docs/modules/modules.hpp
   :language: c++
   :lines: 1-6,8-

.. note::
   
   Most libraries include more than one module, and given that declaring a 
   module requries a single line, it is not uncommon to declare multiple modules
   in a single header file.

Defining the Module
===================

The definition of our ``CoulombsLaw`` module goes in a source file. Defining the
module amounts to implementing two functions: the default constructor and the
``run_`` member function. The constructor is where all set-up for your module
occurs; it is also where you can set meta-data associated with your module. The
``run_`` member function is where your module does the work.

Defining the Constructor
------------------------

The SDE provides the macro ``MODULE_CTOR(T)``, where ``T`` is the name of the 
module to aid in defining the module's constructor. At a minimum the constructor
must set the property type(s) that the module satisfies. It is also a good idea
to provide a short description about the algorithm implemented in the module.

For our ``CoulombsLaw`` module a basic constructor looks like:

.. literalinclude:: ../../../tests/docs/modules/coulombs_law.cpp
   :language: c++
   :lines: 6-24

One particularly nice feature of the SDE is that it can autogenerate 
restructured text documentation for modules by scraping the meta-data and fields
of the module. The description we provided serves as a preface to the resulting
page. Some of the other meta-data that module developers can set in the 
constructor includes: a list of citations, the author of the module, and the
version. A full list of meta-data is available (TODO: Add link).


.. note::

   The SDE automatically documents input parameters and submodules so there is
   no need to include these in your description.

.. hint::
    
   The property type will be needed in a few places so it is a good idea to set
   a typedef, this way if you need to change the property type for any reason
   you only need to change it in the typedef.

.. hint::
   
   The raw string literal introduced in C++11 denoted by 
   ``auto str = R"(...)";`` is a great way to write a lengthy description 
   without having to use escapes or other workarounds for line endings.


Defining the Run Member
-----------------------

The ``run_`` member of the module takes a set of inputs and a set of submodules,
and returns the property (or properties). Like the module constructor, the SDE 
provides the macro ``MODULE_RUN(T)``, where ``T`` is the name of the
module to aid in defining the module's ``run_`` member function. By convention
(and hidden in the definition of the macro) the names of the two arguments are: 
``inputs`` and ``submods``. Presently we will ignore the ``submods`` argument.

For defining our ``CoulombsLaw::run_`` member the relevant SDE bits of code 
are:

.. literalinclude:: ../../../tests/docs/modules/coulombs_law.cpp
   :language: c++
   :lines: 26-27,50-52

The inputs to a module are type-erased. Getting them back in a usable typed form
can be done automatically via template meta-programming. The details of this
process are beyond our present scope; for now know that every property type
defines a static function ``unwrap_inputs`` which takes the type-erased inputs
and returns an ``std::tuple`` with the typed inputs (in the above example we
use C++17's structured bindings to automatically unpack the tuple). In a similar
vein the results of every module are also type-erased and every property type
defines a static function ``wrap_results`` which takes the typed results and 
returns a result map with the type-erased results.

The full definition of the ``run_`` member (including the source code for
computing the electric field) is:

.. literalinclude:: ../../../tests/docs/modules/coulombs_law.cpp
   :language: c++
   :lines: 26-52