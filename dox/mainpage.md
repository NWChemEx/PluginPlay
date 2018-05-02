Simulation Development Environment
==================================

Getting Started with the SDE
----------------------------

Topics in this section are targeted at users who want to run the SDE and 
possibly dynamically influence the control flow.

- [Terminology](@ref terms) details what SDE specific terms mean
- [Overview](@ref sde_overview) provides a high-level overview of the SDE


Extending the SDE
-----------------

Topics in this section are meant for users who want to write apps and/or make 
their libraries discoverable/usable with the SDE, but don't care about the 
internals of the SDE more than they need to. 

  
Understanding the SDE
---------------------

Topics in this section are meant for users who want to understand how the SDE
works in more detail and developers new to the SDE.

## Discussions of How APIs Came to Be
This subsection of topics is mainly for posterity.  Specifically to remember
what considerations went into each of the major classes involved in the SDE's
API.

- [SDEAny API](@ref sdeany_api) SDEAny is used for type-erasure throughout the
  SDE.
- [Parameters API](@ref parameters_api) Parameters is used for wrapping 
  algorithmic parameters to a module.
- [Module API](@ref module_api) Modules are how the SDE's capabilities are 
  extended.
- [Module Manager API](@ref mm_api) The ModuleManager is responsible for 
  managing the available modules.
- [Cache API](@ref cache_api) The Cache is responsible for managing the module's
  state. 

## Discussions of Essential SDE Operations
This subsection includes topics focused on documenting how some of the key SDE
features work.

- [Parallelization](@ref parallelization) Detailed explanation of the 
  assumptions and considerations regarding parallelization within the SDE.    
- [Control Flow](@ref control_flow) Provides examples of how control flows 
  through the SDE in serial and parallel circumstances.

  
