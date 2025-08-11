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
to address deep-seated problems with how scientific software has historically
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
substantial amount of duplicated work. At the same time few, if any, of
these redundant implementations have been developed in a modular fashion, in
turn perpetuating the cycle :cite:`krylov18`.

After years (decades in many cases) of the above cycle, the average legacy
package contains:

- **Millions of lines of code**. Often this code base is poorly
  documented, lacks consistent formatting, has widely varying quality, has poor test
  coverage, and involves multiple coding languages.
- **Hundreds of features**. Many of these features are loosely integrated silos,
  meaning there are only a handful of developers who know how a feature works and
  these features are only integrated with some of the package's other features,
  *e.g.*, a new optimization routine may only be accessible from the algorithm
  it was written to support.
- **Monolithic code base**. Somewhat ironically, even though features are
  loosely integrated with one another, each feature tends to be tightly coupled
  to the code base, *e.g.*, works by assuming existing quirks of other code,
  relies on a particular global state existing. In turn, even if the code base
  is comprised  of libraries and/or components, these components rarely
  function without the rest of the code.
- **Insurmountable technical debt**. A "just get something working" mentality
  has been used for much of the software's lifetime. Each invocation of this
  mentality has added more tech debt. Unfortunately, tech debt tends to
  compound exponentially.

While modularity is not a silver bullet, many developers acknowledge that if
legacy software had been designed and written in a more modular manner current
development efforts would be easier. At this point adopting a more modular
design requires repaying too much technical debt for it to be practical and
many legacy packages are left with two choices: start from scratch, or continue
to push the current development model to its breaking point.

To be fair there are a lot of other factors which shaped how legacy software
came to be:

- **Poor attribution**. Historically publications are the currency of academia
  and publishing software developments has been difficult. When software does
  get published it's often for a release and the resulting paper has hundreds
  of authors. This provides little incentive for a developer to do more than
  the bare minimum.
- **Funding agency expectations**. Decades of delivering software in a "just
  get something working" state (and often overselling how primetime ready it
  really is) has resulted in funding agencies expecting software on unrealistic
  timelines.
- **Lack of formal training**. Most scientific software is written by scientific
  domain experts who have little to no formal computer engineering skills.
  Historically this means that there has been a large amount of ignorance
  regarding best practices.
- **Not invented here syndrome**. It's embarrassing to admit, but in scientific
  software development there tends to be a heavy bias against using software
  developed externally to the team. Part of this is because such software is
  often used in a "black-box" manner, which can be off putting to scientists
  who want to understand how everything work. Another part of this is a
  belief that other developers produce inferior products.
- **Research is not industry**. There is a prevalent belief throughout academia
  that developing research software needs to play by different rules than
  industrial software development. In many cases this belief stems from the
  fact that academics often occupy many roles other than software engineer,
  notably they are also often users of the software. By contrast, most software
  engineers in industry spend the majority of their time writing software.

To summarize, while legacy software represents a substantial investment in
terms of time and money, in many cases existing legacy packages are
unsustainable. The number of organizations dedicated to developing better
research software is a direct result of not wanting to repeat the same
mistakes moving forward.

.. _what_sets_scientific_software_apart:

************************************
What Sets Scientific Software Apart?
************************************

As suggested by the intro to to this page, there is an increasing interest in
developing better more sustainable scientific software. If we are to capitalize
on these efforts we need to understand what makes developing scientific
software challenging, and how to avoid repeating the problems of legacy
software. With regards to why scientific software is unique:

#. Performance

   - Scientific software is among the most computationally expensive software
     in the world. The high computational complexity of many algorithms means
     that even a small degradation in performance can result in a simulation
     becoming intractable.
   - Often requires high-performance computing
   - Performance is heavily tied to hardware; as hardware evolves so will the
     scientific software.

#. Scientific motivation

   - Software is typically seen as a means to an end and is usually developed by
     the scientists themselves.
   - Benefit to cost ratio of dependencies must be large, i.e., dependencies are
     usually only considered if they save a lot of time, or are very performant.
     In addition, the scientists need to have some level of assurance that the
     dependency will continue to be supported in the future.
   - Most scientists prefer to do as little software development as possible.

#. Dynamic nature of scientific research

   - Scientific research is by its nature highly uncertain. Promising
     avenues may not pan out. Funding sources dry up. New hot topics emerge.
   - Workflows vary widely among researchers.
   - Users may come up with use cases beyond the scope of the original software.
   - Research leads to new quantities of interest, and software needs to be
     extensible to support these new properties.
   - New algorithms for computing a property emerge. The software architecture
     needs to be able to use these algorithms throughout the code.

#. Complex nature of scientific research

   - Scientific simulations of real world phenomenon have many pieces.
   - Science domains are often hard to grasp for non-experts.
   - Current scientific research projects are often multi-disciplinary.

#. Need for rapid prototyping

   - Design space for most scientific algorithms is huge. Need to be able to
     quickly scan this space.
   - Python is at present the *de facto* language of choice for rapid
     prototyping.

#. Decentralized scientific software development

   - Developers are typically spread out across the world, making synchronization
     difficult.
   - The scientific software community encapsulates an entire range of software
     engineering capabilities. Therefore, the quality of contributions and
     software products can vary widely.
   - There is a need to protect unpublished research to ensure publication
     rights. Decentralized development allows you to keep your unpublished
     research separate.

Many of the above considerations can be handled by ensuring a modular code
base. When done well, modularity leads to encapsulation and a separation of
concerns. This in turn makes it easier to refactor code for performance, add
support for new theories, work on a feature without affecting other researchers,
and reuse contributions from other groups.
