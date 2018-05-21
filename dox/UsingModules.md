@page using_modules Using the Module Class Hierarchy

[TOC]

The current page focuses on documentation related to the Module Class Hierarchy,
particularly the ModuleBase, and PropertyBase classes.  The page is largely
intended to showcase how they work, and their APIs.  The target audience is
module and SDE developers.

@section using_modules_acyclic Acyclic Visitor Pattern

One of the overarching goals of the SDE is to be extensible, both in terms of
what properties it can compute and what algorithms can be used to compute said
properties.  By "extensible" we in particular mean: users of the SDE should not 
have to modify its source code to add a property or a new algorithm. This is
admittedly easier said then done as the next section tries to illustrate. 

@subsection using_modules_problem The Problem

The simplest way to add a new property and/or a new method for computing said
property is to simply make a function for that method/property combination.  
This leads to code like:

```.cpp
struct Program {
    double compute_scf_energy(...);
    double* compute_scf_grad(...);
    double compute_mp2_energy(...);
    double* compute_mp2_grad(...);
    double compute_ccsd_energy(...);
    double compute_ccsdt_energy(...);
    ...
};
```

For the most part this is the way most existing electronic structure packages 
operate.  Somewhere there is a static list of functions that can be called and 
based on the user's input the package chooses the appropriate function.  This
design fails to satisfy our extensibility criterion (one has to modify the 
source code to add new properties or algorithms).  Furthermore it tends to 
encourage spaghetti code as attempts to generically manipulate/call these 
functions inevitably lead to if/else trees.  We can do better.

As you scan the list of function names in the above code snippet, you see they 
vary in two "degrees of freedom": the level of theory and the property.  In 
computer science when you need to pick the function to call based on two 
criteria this is called "double dispatch".  Googling C++ double dispatch will 
inevitably lead you to the visitor pattern, which gets us a cleaner solution 
than the above, but still fails to saitsfy the extensibility requirement.  

@subsubsection using_modules_cyclic Normal Visitor Pattern

The (cyclic) visitor pattern works by first noting that C++ supports single 
dispatch (this is what function overloading is) and that double dispatch can be 
accomplished by two single dispatches.  To that end we define a class for each 
of the criteria we are dispatching on.  In our case that is the: module 
(generalizing level of theory a bit) and the property.  Calling our properties
class `PropertyBase` a typical API would be:

```.cpp
struct PropertyBase {
    virtual void accept(SCF& mod) = 0;
    
    virtual void accept(MP2& mod) = 0;
    
    // Rest of levels of theory here
};
```

and calling our module class `ModuleBase` a typical API would be:

```.cpp
struct ModuleBase {
    virtual void compute(Energy& prop) = 0;
    
    virtual void compute(Gradeint& prop) = 0; 
};
```

We would then derive from `ModuleBase` our various methods:

```.cpp
struct SCF : ModuleBase {
    void compute(Energy& prop) override {
        // compute SCF energy
        prop.egy = energy;
    }
    
    void compute(Gradient& prop) override {
        // compute SCF gradient and store in prop
    }
};
       
// Similar for other levels of theory
```

and derive our properties from `PropertyBase`:

```.cpp
struct Energy : PropertyBase {
    double egy;
    
    
    void accept(SCF& mod) override {
        mod.compute(*this);
    }
    
    void accept(MP2& mod) override {
        mod.compute(*this);
    }
};

//Similar for other properties
```

It would be used like this:
```.cpp
Energy e;
SCF scf;
e.accept(scf);
std::cout<<e.egy<<std::endl;
```
Ultimately this works because:

1. `e` is given `scf` and selects the `SCF` overload
2. Inside `Energy::accept(SCF&)`, `scf` is given `e` and selects the `Energy`
   overload 
3. The result is `SCF::compute(Energy&)` is called.

This also explains the origin of the somewhat cryptic name, the `scf` instance
visits the `e` instance before ultimately running its functions.

Although this cleanly separates the algorithms for computing a property from the
definition of the property, extending either the set of computable properties or
the set of methods requires modifying the source code (one has to add a 
function for the new property/method).  In fairness it's not too hard to get 
this down to only requiring modifications to the source code when one adds a 
new visitable class (meaning visitors can be added outside the source code), 
but completely breaking the cycle requires the acyclic visitor pattern described
in the next section.

@subsection using_module_solution The Solution

As stated at the end of the previous solution the technique to break the cyclic
dependency in the visitor pattern is called the acyclic visitor pattern.  
Instead of using the single dispatch technology built into C++ (function 
overloading) it relies on multiple inheritance and side-casting. In this pattern
`ModuleBase` is a place-holder class that serves purely to erase the derived 
class's type:

```.cpp
struct ModuleBase{};
``` 
`PropertyBase` now always takes the visitor via the type-erased base:

```.cpp
struct PropertyBase {
  virtual void accept(ModuleBase& mod) = 0;
};    
```

Then each property implements the accept method by casting to the API that 
implements that module.  For example say `EnergyComputer` is a module that 
computes energies.  It's API is then (note it does **NOT** inherit from 
`ModuleBase`):

```.cpp
struct EnergyComptuer {
    virtual void compute_energy(Energy& prop) = 0;
};
```
 
Then we define `Energy` as:

```.cpp
struct Energy : PropertyBase {
    double egy;

    virtual void accept(ModuleBase& mod) {
        dynamic_cast<EnergyComputer&>(mod).compute_energy(*this);
    }
};
```

Our algorithms would then be defined something like:

```.cpp
struct SCF : ModuleBase, EnergyComputer {
    void compute_energy(Energy& egy) override {
        //Compute SCF energy
        
    }
};
```
More specifically `SCF` would inherit from `ModuleBase` and all algorithmic 
implementations that it defines.

To the user, the usage is identical to the normal visitor pattern:

```.cpp
Energy e;
SCF scf;
e.accept(scf);
std::cout << e.egy << std::endl;
```

However, the way the acyclic visitor pattern works is quite different.  Namely:

1. `e` receives `scf` via its base class `ModuleBase`
2. `e` side-casts `scf` to its other base class `EnergyComputer`
3. `scf` receives `e` via its `EnergyComputer` API.
4. `scf` dispatches to `SCF::EnergyComputer(Energy&)`

Admittedly there is still a coupling (here for example `Energy` is coupled 
to the `EnergyComputer` API).  That said, each module is no required to know 
each property and each property no longer needs to know each module.  It should
also be mentioned that this pattern is type-safe (`dynamic_cast` will return a
null pointer if the conversion is not possible).
 
@section using_modules_module_hierarchy Acyclic Visitor Pattern Applied to SDE

At its heart the SDE uses the acyclic visitor pattern with a few modifications
designed to improve the quality of life of its users.  First, we realize that
every property will simply be implemented as:

```.cpp
void accept(ModuleBase& mod) {
    dynamic_cast<T&>(mod).fxn2call(*this);
}
```

and we instead template `PropertyBase<T>` on the the type it needs to cast to 
prevent users from having to implement this line every time (this also 
requires all functions use the same name, which we have chosen as `run`; 
ensuring that the function always has the same name will allow the SDE to skim 
its return and arguments in turn automating type-casting of internal 
type-erased quantities).  Our `Energy` class from above is now nothing more than
a typdef:

```.cpp
using Energy = PropertyBase<EnergyComputer>;
```

Since `PropertyBase` is no longer a proper base class we have opted to drop the
"Base".  The resulting class is Property and it's templated on the class that
will actually be implementing the routine which computes the property.  The 
next change is that ModuleBase is no longer a trivial class, rather it now
holds meta-information related to the module as well as the module's parameters, 
and submodule call-back points. Generally speaking, the parameters and meta 
information may vary depending on the property computed by the module (*i.e.*
depending on which class our module is side-casted to)

As an advanced note, multiple inheritance complicates things considerably so as
a first pass we'll require all modules to compute only one property.  In theory
this should be readily doable as each property is the result of a different 
equation.  If the need for multiple inheritance actually manifests it is 
possible to add it to the existing Property/ModuleBase hierarchy with API 
breaking changes.

@section using_modules_examples Example Usage

For the most part only module/SDE developers will ever work directly with the
ModuleBase class.  This is because ModuleBase is nothing more than the opaque 
handle to the module.  For the purposes of this tutorial we'll assume that 
the module computes energies and is derived from a class `Energy` which is 
declared like:
```.cpp
class Energy : public ModuleBase {
    virtual double run(const LibChemist::System& sys) = 0;
};
```
Ultimately the only thing to note is that the SDE requires the entry point 
into all modules to be called `run`.  The arguments `run` take may vary among
properties.  Here we have decided that all modules that compute an energy take a
molecular system (as described by the `LibChemist::System` class) and return a 
double (the energy).  The specifics of this API aren't particularly important 
for the purposes of this tutorial, aside from the literal signature.   

Generally speaking, there are two sources of ModuleBase instances: the 
ModuleManager and the `submodule()` member of an already existing ModuleBase.  
The primary difference between the two sources is that, as the name implies, 
the modules returned by `submodule()` are the literal submodules that will be
called by that particular module.  Anyways, once one has a ModuleBase instance 
it is possible to inspect the module via its accessor functions, for example 
inquiring into which submodules it will call can be done like:
```.cpp
auto mod = /*get shared_ptr<ModuleBase> from somewhere*/

// Get a read-only list of the module's submodules
// Note: similar calls exist for parameters and meta data
auto submods = mod->submodules();
```

Conversely, we can also change the submodules (or the parameters) for the module
via the setters:
```.cpp
// Change the submodule a module will call:
mod->change_submodule("key", ptr_2_new_submodule);
``` 
Whereas inspecting the module's parameters/submodules is always possible, 
changing the values can only be done when the module is in the un-locked state.
Attempting to change a locked module will cause an error.  Of course this 
means we need a way to inquire into the locked-ness of the module:

```.cpp
if(!mod->locked()){
    // Change settings
}
```
It should be noted that in a threaded environment one needs to atomically check
the lock and modify the settings, otherwise data races may actually cause the
module to get locked between the check and the change.  This data race is also
trivially avoided by doing all parameter modifications at the beginning in a 
serial step.  Once a module has been locked it can not be unlocked.  To use that
module, with different parameters or submodules, one needs to make a new 
(unlocked) copy of that module (this is done via the ModuleManager).  Ultimately
locking turns a module into a read-only state, and greatly simplifies the 
memoization process in parallel environments.

Now that we know how to use a module, it's time to see how to run one.  This
is done via the Property class.  To run an `Energy` module:

```.cpp
std::shared_ptr<Energy> mod = /* get from either ModuleManager or submodules*/
Property<Energy> prop(mod); //Make the Property instance

LibChemist::System sys; // Prepare the input somehow

double egy = prop(sys); // Call the module
``` 

In addition to actually performing the computation, the Property class also 
allows one to perform more fine-grained introspection, such as inquiring into
how long a module will take to run, how much memory it will require, *etc.*. 
All of these introspections can be accomplished *via* the Property::cost 
function.

@subsection using_module_example_python Python Usage

Assuming one has built the Python API for SDE it is possible to use modules 
directly from Python.  Assuming the `Energy` class from the previous section 
has been exported to Python it is possible to implement new `Energy` modules 
or call existing ones directly from Python.  Once one has the module, one can
interact with the module in a manner that is largely identical to C++:

```.py
mod = # get module from either ModuleManager or submodules
submods = mod.submodules() # gets submodules in Python dictionary

if not mod.locked():
    mod.change_submodule("key 2 change", new_submodule)
    mod.lock()
    
    
``` 

Arguably the biggest difference between the C++ and Python module bindings 
comes from the fact that Python isn't templated.  More specifically, 
`Property<T>` must map to a unique Python class for each `T`.  The convention is
to export `T` as `T` (*e.g.* `Energy` for `T=Energy`) and to export 
`Property<T>` as `PropertyT` (*e.g.* `PropertyEnergy` for `T=Energy`).  This 
means that the actual procedure to call a property is slightly different in
Python:

```.python
mod = # get from somewhere
prop = PropertyEnergy(mod)
sys = # get molecular system somehow
egy = prop(sys) # compute energy
``` 


