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

.. literalinclude:: ../../../../tests/pluginplay/docs/modules.hpp
   :language: c++
   :lines: 17-20,26-27,32

specifically we need to include a ``template<typename FloatingPointType>``
statement before the call to ``MODULE_RUN``
(``template<class FloatingPointType>`` would work too). The more significant
changes are to the definition of the module where we have to use different
macros: ``TEMPLATED_MODULE_CTOR`` and ``TEMPLATED_MODULE_RUN``. In addition to
taking the name of the module, these macros also take a list of all template
parameters.

.. literalinclude:: ../../../../tests/pluginplay/docs/templated_coulombs_law.cpp
   :language: c++
   :lines: 17-21, 38-45,51-54,74-76
