Running An App
--------------

This page details what happens when a user calls `sde.run()` with some app key 
`key`.  The entire process is detailed in the following flow diagram:

![](uml/calling_an_app.png)

Step 1: Lock App
----------------

With so much of the SDE's design relying on memoization one challenge is 
determining if two invocations of an app will produce the same result.  This 
is greatly simplified by "locking" the app.  More specifically, the first thing
`sde.run(key)` does is call `sde.play.lock(key)`, which in turn prohibits any 
modifications to the AppInfo instance associated with `key` as well as the 
AppInfo instances of any subapps `key` depends on (*i.e.* they are locked as 
well).  Consequentially, after this step all calls to `key` are 
guaranteed to generate the same output (barring malicious app-developer 
intent or a non-deterministic app; the latter is accounted for in step 2).

Step 2:  Assess if the App is Deterministic
-------------------------------------------

Locking the key guarantees us the same result 99.9% of the time.  The 
exceptional 0.1% arises from when the app is non-deterministic.  By default we
assume an app is deterministic; if it is not, the app developer is 
responsible for setting the `non-deterministic` trait in the app's AppInfo 
instance.  It should be noted that an app is considered `non-deterministic` 
if it calls any `non-deterministic` app.

Step 3: Memoization
-------------------

Once we know if 
