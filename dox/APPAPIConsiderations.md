APP API Considerations
======================

A lot of considerations have gone into designing the app API.  Summarily that
API looks like:

```.cpp
ResultMap operator()(SDE&, const LibChemist::System&, const Parameters&);
```

where:
 - `ResultMap` is a slightly decorated associative array between 
descriptors of a result (as strings) and the result (in a type-erased form)
- `SDE` is the API to the framework providing access to the runtime
- `LibChemist::System` is the description of the chemical system we care about
- `Parameters` is a class for holding the input to the app
  - Described in detail [here](dox/parameters.md).

Simplicity
----------

Apps are essentially call backs and can then take the form of any callable 
object (function pointers, functors, visitors, or lambdas).   Support for 
lambdas in particular is appealing as it allows users of the SDE to 
quickly write one-off apps.  This desire means that apps must have the API:

```.cpp
T operator()(Args&&...args);
```

That is they take some unspecified number of arguments of unspecified types and
return an object of unspecified type.

One App Type or Many?
---------------------

One can imagine a whole hierarchy of app signatures each being a specialization 
of the signature in the last section.  Indeed such a hierarchy is typical for 
C++ plugins as it offloads some error checking to the compiler.  Specifically, 
say you need an app to compute a tensor.  If the return type is 
`TAMM::Tensor<T>` you know the app computes a tensor.  Whether that tensor is
the tensor you want or not would typically be handled by some sort of tag 
system.  The most natural when you expect the tag system to expand (and you 
want the compiler to check it) is strong types (*e.g.* for the Fock 
matrix you define an empty struct `FockMatrix{};` and the return type would be
`std::tuple<FockMatrix, TAMM::Tensor<T>>`).  Enums could be made to play a 
similar role, but require the list to be expanded when additional tags are 
added.  Strings are the most versatile tag, but come with no compile-time
checking.
 
The extreme alternative to a well-defined hierarchy of app types is to 
instead have a single app type that relies on type-erasure for both the input
and outputs to the app (note that templating ultimately would lead to a 
hierarchy, one for each specialization).  The resulting signature would be 
something like `std::vector<std::any> operator()(std::vector<std::any>)` where
`std::any` is the C++17 class for type-erasure (we'll use the `Any` class 
in Utilities to avoid a C++17 dependency).  Of course this suffers from the 
problem that we're not guaranteed to be getting back the quantities that we 
think we are (now both in concept and type).  Like above we can use tags to 
check the concepts for consistency (enums and strings being the only 
relevant options).
 
At least for the time being it's not clear what the minimum set of app APIs 
would need to be.  Thus the type-erasure route seems more prudent.  We'll 
temporarily call the container holding the type-erased instances 
`TypeErasedContainer` and we'll assume it's associative with the keys being
strings.  This suggests our app API should be:

```.cpp
TypeErasedContainer operator()(TypeErasedContainer);
``` 

Minimum App I/O
---------------

Conceptually the app API will allow an app to take anything and return anything.
This is the most generic single signature possible.  As all apps can trivially 
satisfy this signature it guarantees that our program will compile given any 
app.  Strictly speaking it moves nearly the entirety of the app's API to 
documentation (*i.e.* apps need to define what arguments need to be in the input
container and what arguments need to be in the output container as well as the 
types they are expected to be).  Although such APIs are fairly standard in 
other languages, C++ tends to favor APIs that are compiler enforceable.  For 
this reason we want the API to explicitly take enough information for any app to
run without having to pass critical input through the type-erased class.

The first crucial piece of information an app needs is the framework.  The 
framework is the app's means of communicating with the outside world.  The 
class that implements the software abstraction of the framework is SDE.  
Consequentially, we expand the signature to:
 
 ```.cpp
 TypeErasedContainer operator()(SDE, TypeErasedContainer);
 ```

Although we typically think of things like the Fock matrix as being a function 
of the electronic density or the MO coefficients, the reality is both of those 
things in turn depend on the system.  We assert that within a computational 
chemistry package, the result is ultimately a function of one variable, 
the molecular system.  For this reason the second argument to our app is the
system:

```.cpp
TypeErasedContainer operator()(SDE, LibChemist::System, TypeErasedContainer);
```

Strictly speaking although the algorithms are only functions of the system, they
depend on a lot of parameters.  It is the job of the `TypeErasedContainer` 
instance passed to the app to hold these parameters thus we will call it the 
`Parameters` class.  

Up until this point we have had the type erased input and output containers be
the same class.  In reality there are different restrictions on each.  For the
input we have:

- Needs to be checked (*i.e.* not all inputs are valid)
- Usually will have some defaults
- Needs documented for reproducibility
  - Don't necessarily want full structure 
    - Same big matrix input to lots of functions
    - Suffice to have object hash or app key? 
- Used in memoization

For the output:

- Theoretically will always be good so no check needed
  - Algorithm should throw (thus not returning) if there's an error
- Typically needs archived for data analysis
  - Full data structure, with available precision
- Result of memoization

These lists seem like enough justification for two classes (although they 
both will in some shape or form need a type-erased map).  Thus we define the 
(currently opaque) output class to be ResultMap and the input class remains 
of type Parameters.

The final I/O consideration is the qualifiers on the inputs (the output is 
passed by copy which should be elided in presumably all cases).  Generally 
speaking, the SDE will need to be modified in response to (in particular the 
cache and the streams, but also conceivably the parallel runtime and app 
store) and we won't want to copy it; *i.e.* it should be passed by reference.  
The system and the parameters should be read-only as they are strictly 
input; for now we leave them as constant references to avoid copying them in 
deeply nested app calls.  It should be noted that thread-safety for all inputs
will need to be considered later.

Ultimately, with qualifiers our app API looks like:

```.cpp
ResultMap operator()(SDE&, const LibChemist::System&, const Parameters&);
``` 

TODO: Is the Parameters class too heavy in some situations?  In particular 
shell quartets? Is it's cost acceptable if we go in batches? 
    
Access to Information
---------------------

The app API so far assumes all input comes from one of three places:

1. the SDE
2. the molecular system
3. the parameters

Making every app start from this set of input would be impractical in most 
cases.  Instead, we expect apps to piggy-back off of other apps.  For example,
say your app, `A`, needs to compute some quantity `X` and it's easy to 
compute `X` if you're given some other quantity `Y`.  Now assume there's 
some app `B` that can compute `Y`.  To implement your app  you just call `B` to
get `Y` and then use `Y` to make `X`.  Note that since all apps have the same 
API, all `A` has to do is forward its input to `B`.  

Although this sounds simple realize that typically this is not how things are
done.  The reason is `B` may take a very long time to compute `Y`.  Thus what
typically happens is one passes `Y` to `A` to capitalize on the fact that `Y` is
sitting around.  The SDE uses a technique known as memoization to avoid 
recomputing `Y` every time `B` is called.  Basically, only the first call to `B`
will actually compute `Y` all other calls simply return the computed value 
(this is presently done with shared pointers so that we don't have to worry 
about `Y` getting deleted while someone else is using it).

This raises the question of when does information come from subapps and when
does it come from the Parameters instance?  The rule-of-thumb is if it depends
on the molecular system it comes from a sub app.  Otherwise it comes from the
Parameters instance.  Strictly speaking there's nothing stopping a developer 
from ignoring this rule and passing whatever they want through the Parameters
instance.  The main deterrent for this is that it makes your app harder to 
compose with.  More specifically if all apps usually get our hypothetical 
quantity `X` by calling an app and your app decides it wants to get `X` from
the Parameters instance, then any app that calls your app will have to go out
of its way to call an app to get `X` just so it can pass it to your app.  
Likely this means an if/else statement like "If I am calling the app that can't 
follow directions, compute `X`, otherwise, just call the app.". Such an 
if/else statement introduces coupling between the apps and should be avoided.

The main exception to the rule-of-thumb is when an app is going to be called in
a loop and the input that changes is **not** the system or a typical parameter.
An example is the SCF algorithm, in which the goal is to converge some matrix
**D**, which depends on the molecular system, in an iterative fashion.  In this 
case, it makes sense to let **D** be a parameter to the app.

Python
------

As detailed so far the app API relies heavily on type-erasure.  This doesn't 
play well with Python.  Specifically Python can't call the generic version of 
a templated function.  This in turn means you need to enumerate every 
possible conversion like:

```.cpp
pybind11::object at(Parameters p, string type, string key) {
    if(type == "int") {
        auto result = p.at<int>(key);
        return pybind11::cast(result);
    }
    else if(type == "double") {
        auto result = p.at<double>(key);
        return pyind11::cast(result);
    }
    else if...        
}

```

Type-erasure is too convenient to not use.  Thus we have to establish some 
maximum set of types.  Tentatively I'm thinking:

- int
- double
- string
- vector<T> (T=int, double, string)
- map<T, U> (T,U=int, double, string)
- LibChemist::System
- LibChemist::BasisSet
- TAMM::Tensor<T> (T=double others to follow)

The need to handle the last three in particular comes from type-erasure on the
return types.  Generally speaking molecular systems, basis sets, and tensors
should **not** be in Parameter instances. 

TODO: Is there any other common input that doesn't fit on this list?

Example Use Cases
-----------------

### Single Point

The simplest use-case is a single-point (*i.e.* the computation of the energy
of a molecular system at a particular geometry).  Using the SDE this looks like:

```.cpp
//Make an empty SDE instance
SDE sde;

//Assume that functor is a functor implementing the app API for the method that
//will compute the energy and info is its AppInfo instance
sde.play.add_app("NWX", "SCF", "SCF", functor, info);

//Note steps up to here would be typically handled by initialize function

//User creates a molecule somehow
auto sys = fxn_that_makes_molecule();

//Run the computation
auto result = sde.run("SCF", sys);

//Assume energy is under key "Energy"
auto opt_egy = result.at<double>("Energy");
```
 
### Geometry Optimizer

This use-case illustrates what it looks like when apps are nested.  In a 
geometry optimization we are given a molecular system. We then attempt 
to find the configuration (*i.e.* atom placement) of that system that has the
smallest energy as computed by a particular method.

For our The top-level program would look something like this:

```.cpp
//Make an empty SDE instance
SDE sde;

//Assume that functor is a functor implementing the app API for an optimizer
//and that info is the AppInfo instance for it
sde.play.add_app("NWX", "Optimizer", "Optimizer", functor, info);

//Similar assumptions here, except now for the method that will compute the 
//gradient and energy
sde.play.add_app("NWX", "SCF", "SCF", scf_functor, scf_info);

//The derivative order is a parameter so copy the app and change value
sde.play.copy_app("SCF", "dSCF/dX");
sde.play.change_option("dSCF/dx", "deriv", 1);

//Note steps up to here would be typically handled by initialize function

//User creates a molecule somehow
auto sys = fxn_that_makes_molecule();

//Run the optimization (see below for sample optimizer code and param 
//explanation)
auto result = sde.run("Optimizer", sys, Parameters{"Function", "SCF",
                                                   "Gradient", "dSCF/dX"});

//Assume the optimized molecule is returned under the key "System"
auto opt_sys = result.at<LibChemist::System>("System");

//Assume energy is under key "Energy"
auto opt_egy = result.at<double>("Energy");
```

Glossing over the details of how to optimize, the implementation of our app may
look something like:

```.cpp
struct Optimizer {
    ResultMap operator()(SDE& sde, const LibChemist::System& sys, 
                         const Parameters& params) {
                         
        //As part of our app's AppInfo instance we specified that "Function" and
        //"Gradient" respectively be set to the keys of apps that can compute
        //the value of our function and it's gradient (when given the system)
                                              
        auto egy_key = params.at<std::string>("Function");
        auto grad_key = params.at<std::string>("Gradient");
        
        System new_sys{sys};
        bool converged = false;
        while (!converged) {                                                 
            auto egy = sde.run(egy_key, new_sys);
            auto grad = sde.run(grad_key, new_sys);
        
            //Use egy and gradient to compute new system
            new_sys = update_system(new_sys, egy, grad);
            
            //Determine if converged
            converged = check_convergence(new_sys, egy, grad);            
        }; 
    }
};
```
