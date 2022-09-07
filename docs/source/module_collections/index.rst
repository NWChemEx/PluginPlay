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
Module Collections
##################

Typically developers write more than one module. When modules share some common
theme (for example they all relate to computing electric fields) developers will
often choose to distribute/bundle these modules together. We term such
distributions "module collections". The grouping of modules into collections is 
purely arbitary and for the developer's convenience. Come runtime, the set of 
modules available to the user wil be the union of all module collections that 
have been loaded.

The sections in this chapter focus on best practices for maintaining a module
collection. 

.. toctree::
   :maxdepth: 2
   :caption: Contents:
   
   overview
   load_modules
   document_modules
