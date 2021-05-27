******************************
Advanced Module Implementation
******************************

.. |e_field| replace:: :math:`\vec{E}\left(\vec{r}\right)`
.. |velocity| replace:: :math:`\vec{V}\left(\vec{r}\right)`
.. |force| replace:: :math:`\vec{F}\left(\vec{r}\right)`
.. |vec_r| replace:: :math:`\vec{r}`

The previous page covered the basics of writing a module, which is fine for
many applications. For more complicated algorithms however, there are additional
considerations. That is what this page focuses on.

Module-Specific Inputs
======================

Continuing from our electric field example, the factor of: 
:math:`||\vec{r} - \vec{r}_i||^2` means that point charges which are far from
the evaluation point, :math:`\vec{r}`, minimally contributes to the electric 
field (assuming all point charges have approximately the same charge). 
Consequently, we can reduce the computational prefactor slightly by only 
considering point charges "near" :math:`\vec{r}`. What constitutes "near" will
in general be situational (*i.e.*, depends on magnitudes of point charges
involved, target precision, *etc.*). The point of all this is that for
computational reasons, we would like to add a threshold paramter to our electric 
field module.

For simplicity we will implement a new module ``ScreenedCoulombsLaw`` rather
than modifying ``CoulombsLaw``. Relative to our ``CoulombsLaw`` module, the new 
parameter slightly modifies the constructor and the ``run_`` member functions. 
With the new input the constructor becomes:

.. literalinclude:: ../../../tests/docs/screened_coulombs_law.cpp
   :language: c++
   :lines: 23-30

Specifically we declare a new double-precision input called "threshold", with a
brief description and a default value set to the maximum double value (*i.e.*,
no screening will occur by default). The relevant change to the ``run_`` member
is:

.. literalinclude:: ../../../tests/docs/screened_coulombs_law.cpp
   :language: c++
   :lines: 32-37,59-61

Here the point to note is that our module is responsible for manually unwrapping
the new input parameter. It will **NOT** automatically be unwrapped by the
property type.

.. note::
   
   Module-specific inputs work best for parameters that are typically set in
   advance versus parameters that would typically be passed as an input.
   


Templated Modules
=================

Our ``CoulombsLaw`` module hard-coded the floating type to ``double``. In 
practice we can get a factor of 2 speed-up by using ``float`` instead of 
``double``. The trade-off is that ``float`` has reduced precision and is thus
not applicable in all situations. Nonetheless, if we want to support both 
``float`` and ``double`` the easiest way to do this is by templating our module
on the floating-point type. To this end, this section looks at how to write a
module ``TemplatedCoulombsLaw``, which is templated on the floating-point type.

.. note::

   In practice, rather than declaring a whole new module 
   ``TemplatedCoulombsLaw`` you would probably just modify the ``CoulombsLaw``
   module; however, in order to maintain two separate tutorials we opted to
   create a new module.

Templating slightly changes how the module is declared:

.. literalinclude:: ../../../tests/docs/modules.hpp
   :language: c++
   :lines: 1-4,10-11,16

specifically we need to include a ``template<typename FloatingPointType>`` 
statement before the call to ``MODULE_RUN`` 
(``template<class FloatingPointType>`` would work too). The more significant
changes are to the definition of the module where we have to use different
macros: ``TEMPLATED_MODULE_CTOR`` and ``TEMPLATED_MODULE_RUN``. In addition to
taking the name of the module, these macros also take a list of all template
parameters.

.. literalinclude:: ../../../tests/docs/templated_coulombs_law.cpp
   :language: c++
   :lines: 1-5, 22-29,35-38,58-60
   
Iterative Modules
=================

Modules that are meant to be called as part of a loop (typically when optimizing
a quantity) require special consideration. Generally they will have APIs which
take the current quantity and return an updated version of it. 


Satisfying Multiple Property Types
==================================

Modules are allowed to satisfy multiple property types. In this case the inputs
and results are the union of the inputs and results from each property type, as
well as any module-specific inputs.

TODO: Add examples