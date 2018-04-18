@page mm_api Module Manager API

The present page describes notes and thought processes related to the 
formulation of the ModuleManager class.

[TOC]

@section mm_api_use_cases Use Cases

The ModuleManager class is conceived as being the automated overlord of the SDE.
The mechanisms available to it are the same mechanisms available to module 
developers looking to maximize performance.  To that end, the ModuleManager 
class's use cases and users are depicted in the following use case diagram.

![](uml/ModuleManager_use_case.jpg)

The following subsections describe the use cases in more detail. Ultimately 
the use cases suggest that that the ModuleManager API consider the following:

- Possible to register modules without modifying the SDE library.
  - Registering requires making the ModuleManager aware of algorithmic traits as
    well as traits of the results
- Forward arguments from user to module
    - Hash arguments for memoization before calling module
    - SDE instance needed for module and for memoization    
- Saving/loading requires an archive to serialize the Cache instance to    
      

@subsection mm_api_register Registering Modules

Before a module can be used the ModuleManager will need to be made aware of the
module.  This process, called registering has two basic parts: making the 
ModuleManager aware of the algorithm (the literal function to be called) and the
results the algorithm generates.  *A priori* it is difficult to conceive of all
possible properties and algorithms that a program may want to compute.  For this
reason it is essential that it is possible to register new modules without 
having to modify the source code of the SDE.

Exactly what this entails for the ModuleManager depends a bit on which module
API one chooses. Assuming one uses one of the multiple inheritance APIs it 
suffices for the ModuleManager to take modules by the common ModuleBase base
class.  This will implicitly register the properties it brings to the table too.
Internally we will want to store the `type_index` of each property  


@subsection mm_api_call Calling Modules

In addition to registering modules the other primary use case for the 
ModuleManager class is to facilitate running modules.  The actual API for 
running a module must somehow give the required arguments to the module.  
Following from [Module API Considerations](@ref module_api_considerations) we 
also see that this API must support providing the ModuleManager with all 
arguments required for memoization.  At the moment these two requirements 
amount to the same thing: passing an SDE instance and the arguments for the 
module to the ModuleManager.

@subsection mm_api_save Saving/Loading Module State

In most computer programs the ability to save and load program state is a given.
Historically in computational chemistry this has not been the case.  One of the
use cases of the SDE is to rectify this.  To that end we note that it is the 
ModuleManager which stores the "program" and is thus in a position to wrap calls
to automatically save results.  Ultimately this is done via the Cache (details 
of which can be found [here](xxx)).  From the perspective of the ModuleManager,
to save/load the Cache the ModuleManager needs an archive to serialize to/from.
In order to automate this process the ModuleManager also needs to know how often
it should save the Cache.

@subsection mm_api_dag Assemble the DAG

The call tree of a program amounts to a direct acyclic graph, or DAG.  This DAG
starts from the first function call and continues to branch at every nested 
call.  So long as modules expose the list of submodules they are going to call
