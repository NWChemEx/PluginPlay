Understanding the Cache
=======================

Much of the SDE's features rely in some shape or form on the cache.  The purpose
of this page is to detail the inner-workings of the cache in more detail.

Basic Description
-----------------

The Cache is at its heart nothing more than an associative array between unique
result identifications (as determined by hashing the input to an app) and the
values returned by an app.  Cache instances are unique per app type *i.e.* 
instances of the same app share the same Cache.  Under normal circumstances 
the SDE will first attempt to return a cached result before invoking an app. If 
the app is invoked, the result will be saved so that further calls will avoid 
calling the app.  Consequentially combined with the AppInfo instances of 
each app, the Cache is capable of serving as a complete record of what was done
over the course of a calculation and plays an integral role in data archival.   

Interactions with the Cache
---------------------------

The Cache is a private member variable of the SDE so as to avoid meddling 
from users and apps in the memoization process.  Thus all interactions with
the Cache, in some form or another, are carried out through the SDE.  The 
available Cache interactions can be summarized as: 

1. Archiving/checkpointing 
   - Checkpointing saves intermediates and results, archiving only saves results
   - Archiving automatically done when SDE is destroyed
   - Can be automatically done on periodic basis
2. Memoization     
   - SDE checks cache for result prior to calling the app.  
   - If the app needs to be run, the SDE ensures the result is cached afterwards
3. Memory management
   - Essentially all large objects reside in cache
   - Should be directed by user

[Advanced App Flow](dox/AdvancedAppFlow.md) shows where these steps occur during
an invocation of `SDE::run` (for checkpointing and memoization).  

Parallel Interactions with the Cache
------------------------------------

On this page so far we have not worried about using the cache in a parallel 
environment.  When used in a parallel environment things become far more 
tricky.

### Threading

Before starting we should point out that reading/writing to the cache are atomic
operations, *i.e.* it is not possible for for the cache to simultaneously be 
written to or read from by multiple threads.

#### Loading/Saving

Loading a cache is done purely by reading from a Cereal archive.  Cereal only
supports reading multiple archives with multiple threads and thus it is not 
possible to load a single object (the Cache instance), from a single archive.
Consequentially loading of a Cache must always be done in serial.  The 
`SDE::load_cache()` call simply swaps the two instances and assumes it is 
being called from a single thread.

Saving a cache occurs in two places, the first is the destruction of the SDE
instance.  This will be at global scope and should be done in serial.  The 
second is when either the user requests a save or the SDE initiates an 
automatic save.  Regardless of how the save is initiated, `SDE::save_cache()`
will be invoked.  The first thread to enter the function will:

1. Acquire the SDE's mutex (if it doesn't have it already; would have it if
   `SDE::save_cache()` is called from `SDE::run()`)
2. Check if `SDE::cache_changed` is true
   - If not the save is aborted and the SDE's mutex released
3. Acquire the Cache's mutex
4. Save the cache using `SDE::archiver`
5. Release the Cache's mutex
6. Set `SDE::last_backup` to the current time
7. Set `SDE::cache_changed` to false
8. Release the SDE's mutex. 

Note that for the remaining threads this will likely be a no-op as either the
cache will have been checkpointed too recently (if this was an automatic save)
or it will not have changed since the last save (in the rare scenario that
the cache had changed between the two threads entering `SDE::save_cache()` it
would simply be saved again).

#### Memoization

For memoization to be occurring it means we are within `SDE::run()`.  
`SDE::run()` modifies the SDE instance atomically so we only need to worry 
about the consequences of the order threads enter and exit the function.  For 
simplicity we assume we have two threads.

The easiest scenario is if the two threads invoke `SDE::run()` with different
apps.  In this scenario each app works with an entirely different Cache 
instance and the atomic access nature of `SDE::run()` ensures that member 
variables like `SDE::cache_changed` are updated correctly.  The next easiest
scenario is if the same app is invoked with two different inputs.  In this case
both runs will be using the same Cache instance; however, they will be 
writing to different entries of that instance.  Hence, the atomicity of the 
Cache ensures a smooth process.
 
The remaining scenario is if the same app is invoked with the same inputs.  
There are two scenarios that can occur:

1.  Both threads check the Cache before the result has been saved.
  - Result is both threads will run the app and the slower thread's quantity
  will be in the Cache (although it will indistinguishable from the faster's
  result).
  - The check is skipped if app is non-deterministic
2.  One thread manages to finish computing the result before the other checks
 the Cache.
  - In this scenario the slower thread will simply return the cached value.
  - Likely to naturally occur in nested apps with common steps.
  - For non-deterministic apps the slower thread will not check and the result
    of the app will override the Cache entry
    
Summarily, if one threads over apps with different inputs, everything should 
just work.  If the apps have the same inputs, and the app is deterministic it
is pointless to thread over the app (although it will work).  Finally, if the 
app is non-deterministic and has the same inputs only the slowest app's results
will be saved (adding some unique parameter to the input of each call will 
rectify this). 

### Distributed

With the threading scenarios tackled we turn our attention to what happens to
the cache when apps are being run by different processes.
