Printing
========

This directory contains the source code related to printing out modules using
the SDE. The majority of the implementation is hidden in the detail_, namespace
to avoid propagating our dependencies. More specifically, the main API is
simply:

``` C++
std::ostream& operator<<(std::ostream&, const sde::Module&);
```
