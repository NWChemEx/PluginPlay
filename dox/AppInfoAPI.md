@page AppInfo API

This page contains documentation relating to the various metadata and callbacks
an app developer can use to control their app's interactions with the SDE.

[TOC]

@section metadata Meta-Data

For archival and version control purposes one wants to store basic information 
about the module.  The `AppInfo::meta_data` member is a map between recognized 
meta-data and its values.  Generally speaking the data contained in this list
are for users of your app's benefit; that is to say it is not used to influence 
the behavior of the SDE in any way.    

At the moment recognized meta-data includes:

- `name` The name of the app 
  - Meant to be a descriptive name for the user's edification
    - Think "NWChemEx Self-Consistent Field" not "SCF"
  - Primarily for logging and documentation purposes     
  - This does not have to be the name used to register the app with the 
  `AppStore`
- `version` A meaningfully way of summarizing the code state 
  - Ideally follows semantic versioning
    - Semantic versioning: version is "major.minor.patch.tweak"
      - Major incremented when API breaks
      - Minor incremented when new feature added without breaking existing code
      - Patch incremented when bug fixes are added
      - Tweak incremented with every commit      
  - Up to app's developers to maintain
- `description` describes what your app does
  - Should be detailed enough to provide a help/documentation description
  - All apps are used the same way so doesn't need an API documentation
  - Parameter documentation will be generated automatically
- `authors` comma separated list of people to thank/blame for writing the app
- `citations` newline separated list of papers people should cite if they use
  your app

All values are currently restricted to being a single string.

@section traits App Traits

Whereas meta-data did not influence how an app runs, its traits may.  More 
specifically the SDE (and other apps) can request information about your app to
help them maximize performance.  At the moment the list of possible traits 
includes: 

- `thread_safe` your app can be called by multiple threads and behave correctly
- `gpu` your app is able to use a graphics processing unit for performance gains
- `distributed` your app is able to take advantage of being called from multiple
processes for performance gains
- `nondeterministic` your app (intentionally) will not generate the same output
given the same input.
  - Used by SDE to signal that memoization of the app is not possible

In addition to the above traits there is also the `cost()` function that can be 
implemented (either by inheritance or by lambda) to help the SDE and other apps
gauge the amount of a resource your app would use if called.  Currently users of
your app may request estimates of:

- `time` how long your app would take to run given the specified input
  - A value of 0 will be returned for you if the result is cached.
- `memory` how much memory would your app use if it ran

All values computed by your app must be returned in a ReturnMap instance.  The
AppInfo API allows you to declare what your app will return.  Specifically, it
defines a member `returns` which is an associative map between the key your app 
will use to label the return and an `std::pair` whose first element is a 
human-readable description of the return and the second element is a list of 
tags describing the return value.  These tags will be used by the Cache instance
during the memoization procedure.  The list of tags, as well as there 
descriptions, is available [here](@ref cache_memory).
    

@section parameters Parameters
   
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

@section synchronization Synchronization (Distributed Apps Only)

If your app runs in a distributed fashion the SDE will need to synchronize your
app's cache every time it is run on a different set of processes.  This can be
done largely for you; however, if your app returns distributed objects then only
your app is in a position to know what to do with them.
