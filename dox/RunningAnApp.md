Running An App
--------------

The signature for `SDE::run` is:

```.cpp
ResultMap SDE::run(const std::string& key,
                   const LibChemist::System& sys, 
                   const Parameters& params={});
```

This page provides an overview of `SDE::run` assuming it has been invoked 
with the app key `"key"`, the molecular system `sys`, and the input 
parameters `params`.  The following UML diagram summarizes the entire process.

![](uml/calling_an_app.png)

Step 1: Lock App
----------------

With so much of the SDE's design relying on memoization one challenge is 
determining if two invocations of an app will produce the same result.  This 
is greatly simplified by "locking" the app.  More specifically, the first thing
`SDE::run()` does is call `SDE::play.lock(key)`, which in turn prohibits 
any modifications to the AppInfo instance associated with `key` as well as the 
AppInfo instances of any subapps `key` depends on (*i.e.* they are locked as 
well).  Attempts to change the parameters of a locked app result in errors.

Step 2: Merge Parameters
------------------------

For user convenience each app has a default set of parameters.  Those parameters
need to be merged with the parameters the user just provided us.  This occurs
here and is done via a call to `Parameters::insert()`.  Like the associative STL
container Parameters is modeled after, `Parameters::insert()`, only adds unique
elements to the instance.  In particular this means that parameters passed via
`params` will take precedence over the default values.

Step 3:  Assess if the App is Deterministic
-------------------------------------------

By default we assume an app is deterministic; if it is not, the app developer
is responsible for setting the `non-deterministic` trait in the app's AppInfo 
instance.  It should be noted that an app is considered `non-deterministic` 
if it calls any `non-deterministic` app.  Non-deterministic apps can not be 
memoized as the results are not guaranteed to be the same for the same input.
Non-deterministic apps skip to step 6.

Step 4: Memoization
-------------------

Knowing our app is deterministic we call `AppStore::memoize()`.  This function
ultimately will generate a hash (full details [here](dox/memoization.md)) of 
the input.  The hashing algorithm selected is such that a hash collision for 
two different inputs is very unlikely (*i.e.* age of the universe unlikely).

Step 5: Check Cache
-------------------

Given the input hash we then check that app's cache for the hash.  If an 
entry exists under that hash the value is assumed to be the value that will be
computed if the app was actually invoked and returned.

Step 6: Run the App
-------------------

With our memoization attempt thwarted, we are left with no choice but to run the
app.  

Step 7: Cache the Result
------------------------

After the app finishes running the result is cached.  This occurs regardless of
whether or not the app is deterministic.  This is because the cache ultimately
represents the state of the calculation and will be the primary object that gets
archived for data analysis.  Hence any data values wanted for analysis should be
in the cache (including non-deterministic ones).

Step 9: Return Cached Result
----------------------------

The result instance in the cache is from now on considered authoritative.  
This step makes sure that the results to be returned to the user point to the
cache entry.  
