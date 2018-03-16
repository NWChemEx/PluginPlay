Overview of SDE
===============

This page is meant to be a quick introduction to the basic principles underlying
the SDE.  More details on each topic are available elsewhere.

Nature of SDE
-------------

Broadly speaking the SDE is what, in computer science, is called a framework.  A
bit more specifically, the SDE is a framework that is designed to facilitate 
development and extension of computational chemistry codes via an app system.
Under this model, all chemistry functionality is encoded inside a series of 
apps and the user is free to choose which app is called at which time (similar 
to how most modern devices allow you to choose which app is launched by default 
when you click a URL or want to send an email).  Apps are allowed (and 
encouraged) to call other apps to perform subtasks for them. 

Components of the SDE
---------------------

The SDE is decomposed into X major components:

1. Reference data this is the program's API to reference data available to
 the outside world.  Said data includes: physical constants, atomic data, 
 basis sets, force fields, etc.
2. Parallel runtime for the moment this is an opaque object, but eventually it
will contain the software abstractions pertaining to how many threads/processes
are available, what accelerators can be employed
3. Runtime provides abstractions for printing as well as memory management, 
will need to work closely with 2. 
4. Calculation state this includes a list of available apps and results computed
by each app.
