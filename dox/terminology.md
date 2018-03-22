Terminology
===========

This page contains terms that are used throughout this repo as well as 
descriptions of what we mean by them.  Many of the terms are also names of 
classes which represent the software abstractions of these concepts. 

SDE
---

Short for Simulation Development Environment, the SDE is the framework on which
computational chemistry packages are built.  The `SDE` class is the software 
abstraction for interacting with the framework.

App
---

Apps are how the SDE's features are expanded and modified.  Each app is a 
callable piece of code (lambda, functor, or simple function) that adheres to the
required SDE APIs.  See [App Flow](dox/AppFlow.md) for an introduction to how
apps interact with and are used with the SDE.  Generally speaking an app will
typically be the size of one or two functions; however, the app developer is 
free to include as much or as little in the app as they so choose.

Superapps
----------
With apps being envisioned as being roughly the granularity of your typical 
function, we anticipate a typical development team creating many apps.  
Superapps are simply collections of apps (likely from the same developer) 
distributed together.  Typically they will be distributed as a C++ or Python 
library.

AppStore
--------

The `AppStore` is the component of the SDE that stores the details pertaining
to apps and knows how to instantiate a particular app.


Cache
-----

A collection of results obtained from apps that is used for archival purposes
as well as for optimization purposes (usually to avoid recomputing results). 
