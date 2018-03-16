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
apps interact and are used with the SDE.

Superapps
----------
Apps are envisioned as being roughly the granularity of your typical function.  
Consequentially we anticipate a typical development team creating many apps.  
Just like when you get a lot of functions together, with a large number of apps
comes the possibility for the names of apps to collide.  Superapps are to apps
what namespaces are to functions.  Basically they provide an additional 
qualifier that can be used to avoid name collisions while still keeping the 
names short within the qualified space.

AppStore
--------

The `AppStore` is the component of the SDE that stores the details pertaining
to apps and knows how to instantiate a particular app.

Cache
-----

A collection of results obtained from apps that is used for archival purposes
as well as for optimization purposes (usually to avoid recomputing results). 
