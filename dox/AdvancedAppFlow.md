Advanced App Flow
=================

The page [App Flow](@ref md_dox_AppFlow) illustrated the basics of an app's 
lifecycle.  This page covers all of the gritty details.  We attempt to 
reexamine the steps in the same order; however, sometimes it becomes 
necessary to refer to a future or past step.  For this reason it is 
recommended that readers first acclimate themselves with the the app 
lifecycle before tackling this page.  Throughout this page we refer to the 
following UML diagram which shows all relevant parts of the SDE having to do 
with the app lifecycle.

![](uml/app_store.png)

Step 1: App Creation
--------------------

For the most part coding an app up is as simple as detailed on the page 
[App Flow](@ref md_dox_AppFlow).  What we skipped on that page is the large 
amount of metadata that an app developer can attach to their app.  The class 
responsible for this is `AppInfo`.

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
   
The final set of metadata is the settings and the "hooks". Respectively,   

- `AppInfo::parameters`
  - this is a list of inputs like thresholds, number of iterations, *etc.*
  - ideally no part of an algorithm should be hard-coded, but read from here
- `AppInfo::sub_apps`
  - A map such that `sde.run(sub_map["property"], sys)` will run the app
    needed to get the property `"property"`
  - Apps will generally provide defaults for these, but users can override by
    pointing to a different app

Working with the parameters class involves enough detail that it is 
covered [here](dox/parameters.md).  The sub apps property will be covered in 
more detail in step 3.  
   
The SDE is designed so that apps can run without an `AppInfo` instance; however,
the resulting run may be sub-optimal performance-wise.  As added incentive for 
developer's to do a good job filling out `AppInfo` the SDE provides (well it 
can be made to) functions which take an SDE instance, loaded with all the apps 
in their default states, and generates markdown documentation suitable for 
inclusion with Doxygen.

Step 2: AppStore Creation
-------------------------

Step 2 of [App Flow](dox/AppFlow.md) hid a lot of complexity.  Generally 
speaking there are a lot of ways to fill an `AppStore` instance.  The full 
signature of the `add_app` command is:

```.cpp
void add_app(const std::string& super_app_name, 
             const std::string& app_name, 
             const std::strin& app_key, 
             T&& app, 
             const AppInfo& info);
```

For the most part apps will be added to the app store using this full syntax. 
The arguments to which are:

- `super_app_name` essentially the namespace to place the app within
- `app_name` this is the name for the "concept of the" app.  Basically in an 
  object-oriented analogy it's the equivalent of naming the class the defines
  the app.  It can be thought of as defining the fields the app has via the
  `AppInfo` instance.
- `app_key` completing the analogy this is the equivalent of an instance of a
  class.  Basically it has a particular state which may be different than some
  other running version of the same app.
- `app` same as in the basic case, this is the actual callable to use
- `info` this is the `AppInfo` instance defining the parameters and meta-data
  for the app.  

For completeness it's perhaps worth indicating that the shortened version we 
showed you previously maps to:
```.cpp
void add_app(const std::string& app_key, T&& app, const AppInfo& info={}) {
    add_app(app_key, app_key, app_key, app, info);
}
```
and is typically fine for when one wants to write a one-off app, like our 
simple hello world app.  Basically it defines an app with no meta-data and uses
its key for the name of the superapp and the name of the app.

At this point it's perhaps worth explaining why the three strings are in general
needed.

