@page terms Terminology

This page contains terms that are used throughout this repo as well as 
descriptions of what we mean by them.  Many of the terms are also names of 
classes which represent the software abstractions of these concepts. 

[TOC]

@section terms_sde SDE

Short for Simulation Development Environment, the SDE is the framework on which
computational chemistry packages are built.  

@section terms_module Module

Modules are how the SDE's features are expanded and modified.  Each module is
at its heart a callable piece of code (lambda, functor, or simple function) that
adheres to the required SDE APIs.  Modules are relatively coarse-grained in that
typically they wrap routines which compute useful intermediates and important 
results. 


@section terms_module_manager Module Manager


The `ModuleManager` is the component of the SDE that is charged with managing 
all things related to modules.  This includes the list of available modules, how
to instantiate the modules, and meta-data related to those modules. 

@section terms_cache Cache

Ultimately your typical computational chemistry program is little more than a
series of equations to be solved.  The Cache is the object responsible for 
remembering the solutions to those equations.  This serves two purposes: 1) it
is the ultimate record of what was done during the run (what equations were 
solved and what the answers were) 2) it allows us to avoid evaluating equations
for which we already know the answer.  The latter point makes it trivial to 
restart a computation, as all one needs to do is reassemble the equations to be 
solved and then compare against the cache to see which still remain.
