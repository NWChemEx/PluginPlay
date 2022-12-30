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

##################
Module Development
##################

.. |e_field| replace:: :math:`\vec{E}\left(\vec{r}\right)`
.. |r_i| replace:: :math:`\vec{r}_i`

The topics in this part focus on developing modules (as in writing a module, not
as in developing the ``Module`` class). For concreteness, the documentation in
this section assumes we are writing a module that computes the electric field at
a point :math:`\vec{r}`, |e_field|, arising from a set of
:math:`N` point charges. Physically speaking, up to some constants, |e_field| is
given by:

.. math::

   \vec{E}(\vec{r}) = \sum_{i=1}^N \frac{q_i \hat{r}_i}
                                   {||\vec{r} - \vec{r}_i||^2}

where :math:`q_i` is the charge of the :math:`i`-th point charge, |r_i| is a
unit vector in the direction of |r_i|, and the :math:`i`-th point charge is
located at |r_i|. We further assume that there is a header file
``point_charge.hpp`` which defines the class ``PointCharge`` given
by:

.. literalinclude:: ../../../../tests/pluginplay/docs/point_charge.hpp
   :language: c++
   :lines: 17-29

and that the header file ``electric_field.hpp`` defines a property type
``ElectricField`` with an API:

.. code:: c++

   Point ElectricField(Point, std::vector<PointCharge>);

Here the first argument is the point we are evaluating the electric field at,
the second argument is the set of point charges, and the return is the value
of the electric field at that point. Designing property types is a separate
topic covered (TODO: add link).

.. toctree::
    :maxdepth: 2
    :caption: Contents:

    basics
    templated
    iterative
    advanced
    design
