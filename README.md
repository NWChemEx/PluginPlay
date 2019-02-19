Simulation Development Environment (SDE)
=========================================

The SDE represents a reusable framework for building a computational chemistry
program.  In particular it is in the SDE one will find:

- Abstractions of the hardware
  - APIs for I/O, accessing parallel resources, *etc.*
- Abstractions for modifying and monitoring the state of the calculation
  - Current algorithms, calculation state, *etc.*
- Hooks for extending functionality.
- Support for scripting


Building SDE
------------

Please see the documentation (add link when public).

Building Cppyy
--------------

The SDE uses Cppyy to generate Python bindings. At the moment SDE will not build
Cppyy for you. Cppyy can be built manually by using pip:

```
python -m pip install cppyy
```

Pip by default uses your system compilers, so set the `CXX` and `CC` environment
variables appropriately if this behavior is not desired.

