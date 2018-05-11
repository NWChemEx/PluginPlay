@page using_modules Using the Module Class Hierarchy

[TOC]

The current page focuses on documentation related to the Module Class Hierarchy,
particularly the ModuleBase, ModuleImpl, and PropertyBase classes.  For the
most part these classes are not geared at the end-user (aside from actually
computing the property).  The majority of their state and member functions 
instead are designed to facilitate interactions between the module and the SDE. 

@section using_modules_acyclic Acyclic Visitor Pattern

One of the overarching goals of the SDE is to be extensible, both in terms of
what properties it can compute and what algorithms can be used to compute said
properties.  By "extensible" we in particular mean: users of the SDE should not 
have to modify its source code to add a property or a new algorithm.  

@subsection using_modules_problem The Problem

The simplest and most straight-forward type-safe solution is to simply 
list the functions for each available property:

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
source code to add new properties or algorithms).  It's also very clumsy to 
maintain.  We can do better.

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

We would then derived from `ModuleBase` our various methods:

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
the set of methods requires modifying the source code.  In fairness it's not too
hard to get this down to only requiring modifications to the source code for 
either the set of properties or the set of modules (the class visiting needs 
the entire list of visitable classes, whereas the visitable classes interact 
with the visitor generically), but completely breaking the cycle requires the 
acyclic visitor pattern described in the next section.

@subsection using_module_solution The Solution

As stated at the end of the previous solution the technique to break the cyclic
dependency in the visitor pattern is called the acyclic visitor pattern.  
Instead of using the single dispatch technology built into C++ (function 
overloading) it relies on multiple inheritance and `dynamic_cast`.  In this 
pattern `ModuleBase` is a place-holder class that serves purely to erase the
derived class's type:

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

Usage would be something like:

```.cpp
Energy e;
SCF scf;
e.accept(scf);
std::cout << e.egy << std::endl;
```

Despite the fact that the resulting code looks the same the actual implementation
is very different.  Namely:

1. `e` receives `scf` via its base class `ModuleBase`
2. `e` side-casts `scf` to its other base class `EnergyComputer`
3. `scf` receives `e` via its `EnergyComputer` API.
4. `scf` dispatches to `SCF::EnergyComputer(Energy&)`

Admittedly there is still a coupling, but it is only between the property and
that property's API (in our example the `Energy` property was coupled to 
the the `EnergyComputer` API).  Our modules are free to only inherit from some
properties, and need not know all available properties.  Finally, our properties
never need to know the set of modules (here things like `SCF`). 
 
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
preventing users from having to implement this line every time (this also 
requires all functions use the same name, which we have chosen as `run_`).  Our 
`Energy` example from above is nothing more than a typdef now:

```.cpp
using Energy = PropertyBase<EnergyComputer>;
```

Similarly, on the `ModuleBase` side of the hierarchy we introduce 
`ModuleBaseImpl<T>`.  Its main contribution is to wrap the actual call to the
module of type `T` so that memoization occurs.  Unfortunately, in order to 
wrap that function we need to know its signature *i.e.* we can't just hide 
the input/output to the function in the property class.  The initial design in 
@ref module_api "Module API Considerations" attempted to get around this with 
meta-template programming.  More specifically I thought I could just strip the 
return and arguments off a pointer to the derived class; however, C++ is too 
smart for that trick and won't let you get the type of a pointer to a derived 
class's member function in the base class (well to define the base class's API 
anyways; you can do it inside a function so long as that function's signature is
known).  The point is, `ModuleBaseImpl<T>` also needs to be templated on the 
return type of the derived module and the types of the arguments to that module.
This only matters when one defines the module types.  For example, when one 
defines `EnergyComputer` in our above example.  This does not matter to `SCF`
which simply inherits from `EnergyComputer`.  More concretely this looks 
something like:

```.cpp
struct EnergyComputer : ModuleBaseImpl<EnergyComputer, double, int> {
    virtual double compute_energy(int) = 0;
};

struct SCF : EnergyComputer, ModuleBase {
     double compute_energy(int) {
         // compute energy and return it
     }
};   
```

The last major change from the acyclic visitor pattern shown in the previous
section is the removal of multiple inheritance.  Multiple inheritance was only
present to allow each algorithm to compute more than one property.  However,
the reality is typically when an algorithm in electronic structure is considered
as computing multiple properties (think SCF, where commonly one assumes you get
the energy and the wavefunction) one of those properties is computed before the
other (in SCF one computes the wavefunction first and then uses that to 
compute the energy).  Because the SDE supports nesting of modules we can simply
write our SCF energy module in terms of the SCF wavefunction module.

@section using_modules_examples Example Usage

As mentioned at the start of this page the class described here are primarily
meant as liaisons between the SDE and the base classes for each property.  
From a user perspective, knowledge of these APIs and how to use them, will only
be needed if one wants to implement their own properties or if one is trying to 
fine-tune module performance. To that end let us first focus on how to implement
a new property.

Let's say we want to implement a property `EnergyDeriv`.  By convention all 
modules who want to compute an energy derivative will need to implement the 
virtual function that is defined in `EnergyDerivAPI`.  Let us assume that this 
function takes an integer for the derivative order and a `LibChemist::System`
describing the molecular system.  The implementation then returns an instance of
type `Tensor`. Under these assumptions we would define `EnergyDerivAPI` as:

```.cpp
class EnergyDerivAPI : 
    public ModuleBaseImpl<EnergyDerivAPI, Tensor, LibChemist::System&> {
    
    virtual Tensor run_(int order, LibChemist::System& sys) = 0;
};    
```

By convention the virtual function must be called `run_`.  If one is defining a 
lot of properties this definition involves a fair amount of boiler plate.  For 
this reason we have defined the macro `SDE_NEW_MODULE_TYPE` such that 
`SDE_NEW_MODULE_TYPE(EnergyDerivAPI, Tensor, LibChemist::System&);` will 
generate the same code as above.  Note that the arguments (and return type) 
are specified with all of the qualifiers (const, references, pointers, *etc.*).
Finally we need to declare the class which will request that an `EnergyDeriv` be
computed:

```.cpp
using EnergyDeriv = PropertyBase<EnergyDerivAPI>; 
```

That's all there is to implementing a new property.  As mentioned the other use
case of these APIs is for performance.  More specifically, the `ModuleImpl` 
class exposes the `cost` function, which allows introspection of the module. 
For example, say you want to know how long it will take to run a module with a
given input.  You can request that module from the ModuleManager and inquire:
```.cpp
auto mod = //get the module
auto t = mod.cost(Resource::time, /* input */);
//Do something with the time
```  

As a slight aside there is one other notable use case for the `PropertyBase` 
class, namely to be the public API into the module.  This is done via the 
`operator()`.  Continuing with our `EnergyDeriv` example above this would occur
something like:
```.cpp
LibChemist::System sys; // get from somewhere
EnergyDeriv egy(/* get EnergyDerivAPI instance from somewhere*/);
auto e = egy(0, sys);
```
