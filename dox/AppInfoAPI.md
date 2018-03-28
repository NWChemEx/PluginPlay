@page AppInfo API

This page contains documentation relating to the various metadata and callbacks
an app developer can use to control their app's interactions with the SDE.

[TOC]

@section Basic Information

For archival purposes one wants to store basic information about the module, 
such as its:

- `AppInfo::name` 
  - Not necessarily the name it will be called by from the `AppStore`
- `AppInfo::version`
  - ideally follows semantic versioning
  - up to app's developers to increment
- `AppInfo::description`
  - should be detailed for reasons explained below 
- `AppInfo::authors`
  - this should be a list of people to thank/blame for writing the app
- `AppInfo::citations`
  - this should be a list of papers to cite if someone uses your app
  
Generally speaking these fields do not influence any behavior of the app 
(other than some possible version checking) in the SDE.  The next set of 
metadata `AppInfo` provides is data that facilitates scheduling and running 
that app:  

- `AppInfo::traits`
  - List of enums describing algorithmic traits of the app (think thread-safe)
  - Full list is here (link to enumeration class once written)  
- `AppInfo::cost`
   - Estimates the amount of resource (think memory or time) app needs
   - Full list is here (link to enum when written)
   
The final two fields of the `AppInfo` class actually influence the runtime 
behavior of the app.  First we have its parameters, which one can think of 
as defining the user-centric api for the app:   

- `AppInfo::parameters`
  - Defines the non-SDE API for your app.
    - Think of a function:
    ```.cpp
    int my_function(int arg1, double arg2);
    ```
    becoming:
    ```.cpp
    ResultMap [](const SDE& sde, const Parameters& params){
        auto arg1 = params.at<int>("arg1");
        auto arg2 = params.at<double>("arg2");
        ...
    }
    ```
  - ideally no part of an algorithm should be hard-coded, but read from here
  - technically no restriction on what is passed, but objects should generally
    speaking be POD data types or STL containers of POD data types.
  - Full details pertaining to Parameters class is [here](dox/parameters.md).

and the subapp hooks:
- `AppInfo::sub_apps`
  - `subapps.at("option key")` is the app key to call when the app asks for
    subapp `"option key"`
  - goal is for keys to be standardized description of the requested routine
