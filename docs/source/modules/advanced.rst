******************************
Advanced Module Implementation
******************************

.. |e_field| replace:: :math:`\vec{E}\left(\vec{r}\right)`
.. |velocity| replace:: :math:`\vec{V}\left(\vec{r}\right)`
.. |force| replace:: :math:`\vec{F}\left(\vec{r}\right)`
.. |vec_r| replace:: math:`\vec{r}`

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

.. literalinclude:: ../../../tests/docs/modules/screened_coulombs_law.cpp
   :language: c++
   :lines: 22-29

Specifically we declare a new double-precision input called "threshold", with a
brief description and a default value set to the maximum double value (*i.e.*,
no screening will occur by default). The relevant change to the ``run_`` member
is:

.. literalinclude:: ../../../tests/docs/modules/screened_coulombs_law.cpp
   :language: c++
   :lines: 31-36,58-60

Here the point to note is that our module is responsible for manually unwrapping
the new input parameter. It will **NOT** automatically be unwrapped by the
property type.

.. note::
   
   Module-specific inputs work best for parameters that are typically set in
   advance versus parameters that would typically be passed as an input.
   
Submodules
==========

Coulomb's Law is pretty simple. Most simulation techniques are much more 
complicated than that and involve an inticate interplay of many different
properties.

As an example say we are writing a module which computes the force of a moving
charged particle at a position |vec_r|, |force|. Assuming the particle 
has a mass :math:`m`, its velocity at |vec_r| is given by |velocity|, and that 
it has a charge :math:`q`, the |foce| is given by:

.. math::

   \newcommand{\force}{\vec{F}\left(\vec{r}\right)}
   \newcommand{\velocity}{\vec{V}\left(\vec{r}\right)}
   \newcommand{\efield}{\vec{E}\left(\vec{r}\right)}

   \force = m\velocity + q\efield

The important point for this tutorial is that :math:`F\left(\vec{r}\right)` can 
be written in terms of the electric field, |e_field|. To capitialize on all the
inovations in computing electric fields, we decide to write our force module in
terms of an electric field submodule.

The declaration of our module is similar to our previous examples:

.. literalinclude:: ../../../tests/docs/modules/modules.hpp
   :language: c++
   :lines: 1-4,8,16

What changes is the definition. In the constructor we now need to declare that
our module uses a submodule and that the submodule must satisfy the property 
type `ElectricField`. This looks like:

.. literalinclude:: ../../../tests/docs/modules/force.cpp
   :language: c++
   :lines: 22-29

The SDE also provide a tag for the submodule (we choose ``"electric field"``) 
the tag ensures that the module can specify and distinguish between multiple 
submodules of the same type. When appropriately named, the tag also aids in
readability of the code. The ``run_`` function for our module looks like:

.. literalinclude:: ../../../tests/docs/modules/force.cpp
   :language: c++
   :lines: 31-43

Of note is the line:

.. literalinclude:: ../../../tests/docs/modules/force.cpp
   :language: c++
   :lines: 37

which says we are running the submodule tagged with ``"electric field"`` as an
``efield_type``. We provide the submodule with the point charge's location and
the list of point charges defining the electric field.

Submodules are integral to the SDE so it is worth noting:

- By using submodules we establish data dependencies (*i.e.* here we stated we 
  need an electric field, we don't care how it's computed)
- If tomorrow someone writes a new electric field module, it can immediately be
  used with our force module without modifying the force module.
- The submodule that is called can be changed at runtime by the user and our
  module doesn't have to maintain the logic to do so (*i.e.* our force module
  doesn't need to maintain a list of all possible submodules and switch between
  them).
- If the calculation crashes after computing the electric field, but before the
  module completes (pretty unlikely given how few operations remain) memoization
  will avoid the need to recompute the electric field when the calculation is
  restarted. The module does not have to maintain separate checkpoint/restart
  logic!!!!!  

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

.. literalinclude:: ../../../tests/docs/modules/modules.hpp
   :language: c++
   :lines: 1-4,10-11,16

specifically we need to include a ``template<typename FloatingPointType>`` 
statement before the call to ``MODULE_RUN`` 
(``template<class FloatingPointType>`` would work too). The more significant
changes are to the definition of the module where we have to use different
macros: ``TEMPLATED_MODULE_CTOR`` and ``TEMPLATED_MODULE_RUN``. In addition to
taking the name of the module, these macros also take a list of all template
parameters.

.. literalinclude:: ../../../tests/docs/modules/templated_coulombs_law.cpp
   :language: c++
   :lines: 1-6, 21-28,34-37,57-59
   
Iterative Modules
=================

Modules that are meant to be called as part of a loop (typically when optimizing
a quantity) require special consideration.


TODO: Finish this section once issue #160 is taken care of