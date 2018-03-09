SDE
===============

The SDE repo is the home of classes and infrastructure that are 
designed to abstract away details pertaining to the literal environment that 
NWChemEx is being run in.  Such details include:

- hardware considerations such as I/O and other memory management (this is a 
stub for now) 
- parallelization considerations not covered by TAMM (also a stub)
- tabulated scientific data such as basis sets, physical constants, *etc.*
- calculation state (the in program representation of the call stack)

Building
--------

SDE relies on CMakeBuild to manage its configuration and build 
settings.  More details on how to build this library can be found 
:link:[here](CMakeBuild/dox/Building.md).
