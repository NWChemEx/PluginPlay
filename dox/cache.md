@page cache Understanding the Cache


[TOC]

Much of the SDE's features rely in some shape or form on the Cache.  The purpose
of this page is to detail the inner-workings of the Cache in more detail.

@section cache_description Basic Description

The Cache is at its heart nothing more than an associative array between unique
result identifications (as determined by hashing the input to an app) and the
values returned by an app.  Cache instances are unique per app type *i.e.* 
instances of the same app share the same Cache.  Under normal circumstances 
the SDE will first attempt to return a cached result before invoking an app. If 
the app is invoked, the result will be saved so that further calls will avoid 
calling the app.  Consequentially combined with the AppInfo instances of 
each app, the Cache is capable of serving as a complete record of what was done
over the course of a calculation and plays an integral role in data archival.   

@section cache_interactions Interactions with the Cache

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
   - May need to stop storing some elements

[Advanced App Flow](dox/AdvancedAppFlow.md) shows where these steps occur during
an invocation of `SDE::run` (for checkpointing and memoization).  

@section cache_memory Memory Management

Results are returned from an app via a ResultMap instance.  This is basically a
type-erased associative array between strings (describing what a result is) and
a shared pointer to the type-erased result.  The shared pointers ensure that 
a result is not freed before all users of the result are done with it.  After 
creation, ResultMap instances are read-only, they are not immutable as various 
operations will modify the reference counts of the shared pointers.  A copy of 
the ResultMap instance is stored in the Cache.  This copy keeps the reference 
count of the shared pointers minimally at one for so long as the Cache exists.

Apps are free to associate tags with each entry of the ResultMap.  Currently the
following tags are available:

- `intermediate`: result is likely only useful for obtaining some other result 
  - Used to indicate result should not be archived
  - Intermediates are checkpointed
- `expendable`: result is unlikely to be useful as an intermediate
  - Not checkpointed or archived
- `no_cache`: result should not be stored in the cache
  - Not checkpointed or archived
  - Cache will not be used to keep reference count at 1    
- `distributed`: result's state is stored in multiple memory spaces
  - Used to indicate result must be synched when processes sets are 
    partitioned.
    
By default a result is assumed to be worth archiving and replicated across all 
processes.

Eventually the plan is to have some "smart" memory management in place whereby
when memory gets tight the SDE deletes its copy of a cached entry (reducing the
shared pointer reference count by 1).  For now the only memory management in 
place is manual deletion.  In particular, a user is free to call 
`SDE::clean_cache()`, which will delete all cache entries associated with the
given app (**not** app instance).  Optionally the user may supply an `std::set`
of ResultTag instances and only cache entries with those tags will
be deleted.  We note that `clean_cache` is meant for use by the user not an 
app.  This restriction is necessary because apps can be nested, which in 
turn means only a user is in a position to determine what can and cannot be 
deleted (well theoretically they are at least, this may not apply to users who 
think there's a flavor of DFT called "the Gaussian DFT"). 

@section cache_parallel Parallel Considerations

On this page so far we have not worried about using the cache in a parallel 
environment.  When used in a parallel environment things become far more 
tricky.

@subsection cache_threading Threading

Before starting we should point out that reading/writing to the cache are atomic
operations, *i.e.* it is not possible for the cache to simultaneously be written
to or read from by multiple threads.  Calls to the SDE behave similarly with the
notable exception that `SDE::run` releases all mutexes before actually calling
the app. 

@subsubsection cache_thread_load_save Loading/Saving

Loading a cache is done purely by reading from a Cereal archive.  Cereal only
supports reading multiple archives with multiple threads and thus it is not 
possible to thread loading a single object (the Cache instance), from a single 
archive.  Consequentially loading of a Cache must always be done in serial.  
The `SDE::load_cache()` call simply swaps the two instances and assumes it is 
being called from a single thread.

Saving a cache occurs in two places, the first is the destruction of the parent
SDE instance.  This will be at global scope and should be done in serial.  The 
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
Cache will have been checkpointed too recently (if this was an automatic save)
or it will not have changed since the last save (in the rare scenario that
the Cache had changed between the two threads entering `SDE::save_cache()` it
would simply be saved again).

@subsubsection cache_thread_memoization Memoization

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
cause the outputs to not clobber each other). 

@subsection cache_distributed Distributed

With the threading scenarios tackled we turn our attention to what happens to
the cache when apps are being run by different processes in a possibly nested 
fashion.  For the purposes of this section we introduce a few definitions:
 
- A "distributed" object is an object whose state is stored across multiple
processes, but is interacted with as if it was local (*e.g.* a tensor where 
each process holds some elements)
- The Cache is "synchronized" for a set of processes, if after calling an app 
the Cache entry for the app is the same replicated or distributed object for 
each process.

It is also necessary to make some assumptions about the ParallelRuntime class.
In particular:

- It is possible to split the processes into subsets
- The subset can be released
- Objects do not keep subsets of the processes after they've been released


@subsubsection cache_dist_memoization Memoization

As a first pass we ignore distributed objects.  To that end, the easiest 
memoization scenario to cover is what happens when say two processes call the
same app with the same input.  If the app is deterministic the output will be 
the same across all processes and consequentially the Cache instances will be
too.  The other possibility is that the two processes call apps that generate 
different outputs.  This can occur if the apps are different, are given 
different inputs, or if the apps are non-deterministic.  This is what 
will happen when coarse-grained parallelism is employed. In these cases it
is always possible to wrap the call in a wrapper app like:

```.cpp
ResultMap operator()(...) {
    //Split processes in sde (syntax forthcoming when ParallelRuntime) defined
    
    std::vector<ResultMap> my_results;
    
    for(auto app_key : my_process_apps_to_run)
        my_results.push_back(sde.run(app_key));
    
    //Synchrnoize my_results across current communicator and return
    return synchronize(sde, my_results);
}
```

The wrapper app makes this scenario appear to the caller as if it is the first 
scenario we discussed in this section and the same analysis applies.  Regardless
of which of the two scenarios actually occurs memoization will work as intended.

The synchronization is generally speaking non-ideal.  This brings us to 
distributed objects and this is where things get complicated.  We want 
distributed apps to look something like:

```.cpp
ResultMap operator()(...) {
    distributed_object my_results;
    
    //Example of using semantics that look local to fill object
    for(auto idx : my_results)
        my_results[idx] = sde.run(idx);
    
    //Return w/o synchronizing
    return {"the results", my_results};
}
```

Note that strictly speaking this app is not given the same input on all 
processes (a distributed object minimally depends on the process ID); 
however, we want the app to act as if it was given the same input.  As long as
subsequent calls to the the app use the same process set, memoization should 
work as expected.  Unfortunately, it is unlikely that all parts of the 
computation will use the same set of processes.  When partionings of the 
process set happen it looks something like:

```.cpp
auto new_sde = // Split SDE processes API forthcoming
    
auto result = new_sde.run(); // Triggers synch from original procs to subset
   
// Release processes back to original set, API forthcoming
// (Triggers synch from subset back to original set of processes)
```

In such a pattern there's two trouble areas: creation of the process subset and
release of the process subset.  In creating a process subset we need to ensure 
that objects that are distributed over the original process set are accessible 
only from the processes in the subset.  In releasing the process subset we need 
to make sure objects that were only accessible on those processes are now 
accessible to the larger process set.  For normal (non-distributed)  objects 
giving them to the smaller process set is a no-op, whereas giving them to a 
larger process set is simply an `allgather`.  Generally speaking, it is 
not possible to do such reasoning with distributed objects.  For this reason we 
introduce the method `AppInfo::synchronize()`  to the AppInfo class.  If a 
result is marked as `distributed` and if that app is being run on a new process
set, then during the line `new_sde.run()` of the above code snippet 
`AppInfo::synchronize()` will be called on it and all subapps.  The 
input to `AppInfo::synchronize()` is the current SDE instance and a Cache 
entry for your app.  It is your app's responsibility to make sure that entry is
accessible from the processes associated with that SDE instance (more details 
about the AppInfo API are located [here](dox/AppInfoAPI.md)). This accounts for 
moving to a subset of processes; the reverse (moving to a superset of processes) 
is triggered when the subset of processes are released back to superset.  With 
process set partitionings covered, memoization will work for distributed objects 
as expected.


To summarize this subsection, apps are responsible for in some shape or form for
hiding distributed calculations.  What this means is the caller of the app 
should not have to do anything special to use your app just because it's 
distributed.  This is envisioned as being largely accomplished by distributed
objects and synchronization of said objects falls to the app developer using
them. 
 
@subsubsection cache_dist_save_load Saving/Loading

For the most part saving/loading considerations for the Cache under 
distributed runs fall to the archive implementation.  Ultimately the Cache 
instance will use the usual Cereal `serialize()` function to 
serialize/unserialize the Cache.  This means considerations such as:

- parallel I/O usage
- how each process's Cache is stored (separately, together, fully synched, ...)
- how stored Cache states are restored

all fall to the archiver.  In particular the archiver must do the above in a 
manner that is consistent with the objects it is storing.  For example, if the 
archiver is given a tensor distributed over two processes, that tensor must 
be stored in such a way that when reloaded the resulting tensor can be used 
identically to the original (assuming processes are split the same way); 
whether that means the data is replicated, redistributed, or something else is 
between the tensor and the archiver.

Arguably the only consideration for saving/loading on the Cache's part is the
process by which checkpointing is done.  In particular, when it is time to 
checkpoint (as determined by either a user's request or sufficient passage of
time) all processes will serialize their current local Cache instance into the 
archiver provided to the SDE.  The archiver is free to block on the full set of
processes given to the SDE because all said processes will eventually 
attempt to save (even if it takes waiting for them to get to the primary SDE 
destructor, where they will infinitely loop around a save call until all 
processes have arrived).  Obviously it is preferred for the checkpointing of 
each process to be non-blocking.

@subsection cache_hybrid Hybrid Parallelism

You thought distributed was complicated?  Now it's time to combine them.  Get
ready 'cause boy are things going to get....easy?  Yeah, actually the choices 
for threading and distributed make them play well together so this section is
short and sweet.

Loading is never done threaded so under hybrid parallelism conditions it's the
same as the distributed case.  Similarly because the save is called from an 
atomic region it's only the distributed parallelism that plays any role here 
as subsequent threads passing through the section will do nothing (unless 
the Cache has changed).  As for memoization, checking the Cache is atomic so the
literal check is only influenced by the distributed parallelism (threads running
the same app, with the same input may get the result there sooner, but that's 
about all threads can do here).  Finally because different threads can only 
meaningfully interact with different entries of the Cache and different 
processes have different instances, saving entries to the Cache for memoization
should just work too.
