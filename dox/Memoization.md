Memoization
===========

A large part of the SDE relies on memoization.  This page discusses those 
efforts in more detail.  Note everything on this page applies to 
deterministic apps, non-deterministic apps can not be memoized.

Assumptions
-----------

- App is deterministic
- App completes successfully
  - Apps that fail should throw and thus not generate results to memoize
- App can only get input from the SDE or Parameters instances it is given 
  (subapps ultimately come from the SDE instance)
  - Apps should not write their own files or write to a global variable. 
  - Parameter instance is unique per key
- To replicate a call to the app it suffices to memoize the output of the app
  - Invoking the app has no side-effects aside from generating the output  

App's Information Lifecycle
---------------------------

To reason about memoization we need to understand the lifecycle of data 
associated with an app.

### Step 1: Default Values

When an app is developed the developer puts all parameters the app depends on in
a Parameters instance in the AppInfo accompanying it.  Each value is in one of 
three states:
1. Not set
   - Values in this state may be either:
     - Optional, the app can run without the value being set
     - Required, the app must have this value set by user to be run
2. Initialized to some default value, or 
3. Initialized to the app key of an app capable of generating a default value

An app will always be originally loaded with its default values.

### Step 2: User Default

When a run starts a user may override one or more defaults for an app.  
Tentatively we assume this is done before any calls to an app.

### Step 3: App Invocation





  
