.. Copyright 2023 NWChemEx-Project
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

###########################
List of PluginPlay Features
###########################

********************
Fundamental Features
********************

The features listed in this section literally define an inversion-of-control
framework. So while they have been available since version 1.0.0, their
fundamental nature makes them almost assumed; nonetheless, we list them here
for completeness.

- Algorithms are written as modules, self-contained encapsulated units of code.
- Interfaces defining how modules exchange data are defined via "property
  types". Users can choose how to call a module dynamically by selecting from
  the property types it satisfies.
- Property types can be developed downstream of PluginPlay and may thus contain
  domain-specific data.

**************************************
Features available in PluginPlay 1.0.0
**************************************

- Memoization of the call graph.
- RocksDB as a cache backend.
- Ability to automatically generate reStructured Text documentation for
  modules.
- Annotated module bounds checking.
