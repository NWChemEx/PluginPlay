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

SDE is built using 
[CPP](https://github.com/CMakePackagingProject/CMakePackagingProject.git).
Assuming you have already installed CPP, that you are on a sane Unix-like 
computer, and you are willing to let SDE build all dependencies, then the 
following will suffice to build SDE:

```
git clone https://github.com/NWChemEx-Project/SDE.git
cd SDE
cmake -H. -Bbuild -DCPP_GITHUB_TOKEN=<your super-secret token> \
                  -DCMAKE_PREFIX_PATH=<where/you/installed/CPP> \
                  -DCMAKE_INSTALL_PREFIX=<where/you/want/to/install/SDE>
cd build
cmake --build .
#May need to run as an admin depending on where you are installing
cmake --build . --target install  
```

The GitHub token is necessary because, at the moment, utilities is a private 
repository (instructions for generating a token are 
[here](https://help.github.com/articles/creating-a-personal-access-token-for-the-command-line/)).

For finer-grained control over the build we direct the reader to the more 
thorough CPP build instructions located 
[here](https://cmakepackagingproject.readthedocs.io/en/latest/end_user/quick_start.html)

and note that SDE depends on several other projects:

- [utilities](https://github.com/NWChemEx-Project/Utilities)
- [bphash](https://github.com/bennybp/BPHash)
- [pybind11](https://github.com/pybind/pybind11)
  - Requires a development version of Python
- [cereal](https://github.com/USCiLab/cereal)
- (For testing only)[Catch2](https://github.com/catchorg/Catch2)


