@page cache_api Cache API

[TOC]

The current page focuses on the the design decisions behind the Cache object.
For the most part the design of the Cache object was driven by the 
[Module API](@ref module_api) and the [Module Manager API](@ref mm_api).

@section cache_api_use_cases

The following use case diagram shows the intended uses of the Cache as well as
who those use cases are primarily targeted at.

![](uml/CacheAPI_use_case.jpg)

For the most part the use cases for the Cache are straightforward.  We want to
be able to checkpoint the program's state and reload it if something goes wrong.
Additionally, we want a means to archive (long-term storage) the results of a 
computation, specifically in a manner that does not sacrifice context or 
precision.  Given the focus on parallel computing, it will be necessary over 
the course of a calculation to synchronize results stored on a superset or 
subset of processes.  The Cache must be able to do this in a somewhat automatic
fashion.  Finally, since the Cache will be the largest consumer of memory it is
essential that the Cache have some routines for reducing its memory footprint.

@section cache_api_options Purposed Cache API

Ultimately the Cache is just a large hash map holding the state of modules
that are accessible from the current ModuleManager instance.  Synchronization,
serialization, etc. is handled by the SDEAny class.  Thus at this point I don't
see any reason to use anything other than an `std::map` (can't use an 
`unordered_map` because the hashes can't be derived from the values).
