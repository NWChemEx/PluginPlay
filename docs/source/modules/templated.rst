*****************
Templated Modules
*****************

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
   
