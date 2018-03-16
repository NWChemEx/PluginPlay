App Flow
========

This page focuses on the basics of the app lifecycle.  It on purpose, skips 
over many of the intricacies of the full app lifecycle, available 
[here](dox/AdvancedAppFlow.md).  

Step 1. App Creation
--------------------

Before an app can be used it must be literally written.  The SDE tries to place
minimal restrictions on what an app must look like.  Ultimately any callable
object (function, functor, or lambda) defining the API:

```.cpp
ResultMap operator()(const System& mol, const SDE& sde, const Options& options); 
```

will work.  Here `ResultMap` is the required return type and is basically a 
type-erased `std::map` (*i.e.* it's a map from strings to objects of arbitrary
type), `System` is the `LibChemist` class that describes the chemical system to
be studied, `SDE` is an instance of the SDE framework, and `Options` is an 
instance of the Options class which stores parameters your app uses.  For the
time-being we ignore the intricacies of app creation by using lambdas.

Step 2 :AppStore Creation
-------------------------

Ultimately all parts of the code will look to the SDE, specifically the 
AppStore, to obtain apps.  Before an app can be used it must be registered with
the AppStore.  To do this given an instance of the SDE, `sde`,  any of the 
following suffice. Assuming your app is a lambda:

```.cpp
sde.add_app("MyApp", [](const System&, const SDE& sde, const Options&){
    sde.stdout << "Hello World" << std::endl;
    return ResultMap("The answer to everything", 42.0);
    });
```

or if it's a function:

```.cpp
ResultMap my_first_app(const System&, const SDE& sde, const Options&) {
    sde.stdout << "Hello World" << std::endl;
    return ResultMap("The answer to everything", 42.0");
}

sde.add_app("MyApp", my_first_app);
```

or if it's a functor:

```.cpp
struct my_first_app {
    ResultMap operator()(const System&, const SDE& sde, const Options&) const {
        sde.stdout << "Hello World" << std::endl;
        return ResultMap("The answer to everything", 42.0);
    }
};
sde.add_app("MyApp", my_first_app{});
```

At this point our app is registered with the AppStore under the key `"MyApp"`.  
This means anytime anyone asks for the app `"MyApp"` they will call your app.
Generally speaking this step can become very verbose as there will be a 
`add_app` call for literally every app.  For this reason this step is usually
done by the initialization function.

As a slight aside, the astute reader/coder will note that because the apps are 
being loaded into an object created at runtime (as opposed to being 
hard-coded calls in a driver-like routine) users are free to modify the list 
of apps at runtime (by say loading a shared library) without having to modify
the source code.

Step 3: Calling an App
----------------------

As alluded to in the previous step we can call any app given its key.  For 
example to call the app we added in step 2:

```.cpp
auto result = sde.run("MyApp", sys);
```

This piece of code would do nothing, but print `Hello World` to standard out and
return a `ResultMap` instance containing the floating point value `42.0` under
the key `"The answer to everything"`, which could be accessed *via*:

```.cpp
auto forty_two = result.at<double>("The answer to everything");
```

Note you will have to know the type and what the app calls the return (the 
app should document this).

Step 4: Destroying the App
--------------------------

Technically this occurred in step 3, but it's worth making this explicit.  The
call `sde.run()` looks internally a little like this:

```.cpp
run(...) {
    auto app = play.at("MyApp"); // I'm an Android person so internally the app
                                 // store is the play store...
    auto result = app(sys, *this, Options{}); //Ignore where options comes from
    return result;
}
```  

What this means memory-wise is that the first line instantiated your app and 
after `sde.run()` returns, the instantiated instance will be deleted.  
Consequentially, if your app computes a useful quantity then that quantity needs
to be in the return.
