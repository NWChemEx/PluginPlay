tests Directory
===============

This directory contains the unit tests and tutorials for the pluginplay repository. The
unit test file/directory structure follows the same structure as the source
code. This directory additionally contains the header file `test_common.hpp`,
which contains facade property types and modules for testing purposes. These
helper classes are summarized below for convenience.

Property Types
--------------

- `NullPT` : The most minimal property type possible
  - API: `void(void)`
- `OneIn` : A property type with one input
  - API: `void(int)`
  - Input key is `"Option 1"`
- `OneOut` : A property type with one result
  - API: `int(void)`
  - Result key is `"Result 1"`
- `TwoOut` : A property type with two results
  - API: `std::tuple<int,char>(void)`
  - Result keys are respectively `"Result 1"` and `"Result 2"`
