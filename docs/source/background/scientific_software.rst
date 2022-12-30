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

.. _developing_scientific_software:

##############################
Developing Scientific Software
##############################

.. warning::

   The authors of this section are primarily developers of electronic structure
   software. The views and anecdotal evidence throughout this section draw
   heavily from this experience. While we believe this page reflects scientific
   software as a whole, the reader should bear this warning in mind.

Starting in the 2010s there have been a number of efforts to improve the state
of scientific software including:

- Better Scientific Software (BSSw) :cite:`bssw`
- The German Society for Research Software Engineers (deRSE) :cite:`derse`
- The Molecular Sciences Software Institute (MolSSI) :cite:`molssi`
- The Nordic Research Software Engineers Association :cite:`nrse`
- Research Software Alliance (ReSA) :cite:`resa`
- The RSE Association of Australia and New Zealand :cite:`rsaau`
- The Society of Research Software Engineering :cite:`rse`
- Software Engineering for Science (SE4Science) :cite:`se4science`
- The Software Sustainability Institute (SSI) :cite:`ssi`
- The United States Research Software Engineer Association
  (US-RSE) :cite:`usrse`
- Working Towards Sustainable Software for Science: Practice and
  Experiences :cite:`wssspe`

These are only some of the more general efforts, within specific domains there
are even more. The point is so many efforts exist because there is a need
to address deep-seeded problems with how scientific software has historically
been developed. So let's look at the typical scientific software package circa
2000.

******************************
Scientific Software Circa 2000
******************************

Many legacy scientific software packages grew organically with little to no
design. In many cases new packages sprung up because existing packages did
not fill a need, or because the existing packages had differing philosophies,
*e.g.*, open vs. closed source, functional vs. object-oriented. While legacy
packages may have had different origin stories, the lack of design,
particularly with respect to interoperability, means that when a feature of
another package was needed it was often easier to re-implement the feature
than to interface to the other package. Hence over the years, each package's
feature set tended to converge to a set of standard algorithms. Each of these
algorithms has now been implemented a number of times, resulting in a
substantial amount of duplicated work :cite:`krylov18`.

After years (decades in many cases) of the above cycle the technical debt has
piled up. While many developers acknowledge the importance of modularity to
avoid the reproduction of work, adopting a modular design often requires
repaying too much technical debt for it to be practical. The existing legacy
software packages


Making matters worse credit for software development is not properly
attributed.
