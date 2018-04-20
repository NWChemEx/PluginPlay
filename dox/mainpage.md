Simulation Development Environment
==================================

Getting Started with the SDE
----------------------------

Topics in this section are targeted at users who want to run the SDE and 
possibly dynamically influence the control flow.

- [Terminology](@ref terms) details what SDE specific terms mean
- [Overview](@ref sde_overview) provides a high-level overview of the SDE
- [User Workflow](dox/workflow.md) details a typical workflow
- [App Workflow](dox/AppFlow.md) details the basics of running an app as well as
  how to write simple "lambda apps"
- [Common Tasks](dox/CommonTasks.md) explicitly details how to perform common 
  computational chemistry tasks within an app  

Extending the SDE
-----------------

Topics in this section are meant for users who want to write apps and/or make 
their libraries discoverable/usable with the SDE, but don't care about the 
internals of the SDE more than they need to. 

- [Advanced App Workflow](dox/AdvancedAppFlow.md) details the app lifecycle in
  much greater detail to ensure users can get the most out of their apps
- [AppInfo API](dox/AppInfoAPI.md) details about the available meta-data options
  for an app  
- [Setting Parameters](dox/parameters.md) details the types of parameters 
  available and the features provided by the Parameters class
  
Understanding the SDE
---------------------

Topics in this section are meant for users who want to understand how the SDE
works in more detail.

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
    

- [Memoization Considerations](dox/memoization.md) discusses the fine details
  of memoization  
- [Running an App](dox/RunningAnApp.md) details what happens when a user 
  calls `sde.run`
- [Cache Considerations](dox/cache.md) everything you wanted to know about the 
  cache (and more) 
  
  
