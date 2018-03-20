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
AppInfo instances of
