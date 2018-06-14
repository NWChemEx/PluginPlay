@page writing_a_module Writing Modules and Module Types

The purpose of this page is teach you how to write a module and, if needed, a 
module type.  As the latter is a prerequisite for the former we start with how
to write a module type; however, if a module type already exists for the 
property your module will compute it is not necessary to write a module type.

@section writing_a_module_type Writing a Module Type

Module types define the APIs for computing a particular property.  For example,
say we want to write a module that computes an energy.  It would be reasonable
to define the energy API such that it takes a molecular system and returns a
double.  Modules capable of computing the energy of a molecular system then 
inherit from the resulting module type and implement the function.  The actual
API is largely arbitrary aside from:

- ModuleBase must be a non-virtual and public base class
- the entry point must be a non-templated, public member function named `run`
- it must be written in C++ and 
- it must take at least one argument 

All restrictions, aside from the accessibility of the base class and a uniform 
non-templated function name, are for convenience and could be relaxed if need 
be.  Anyways, this means that we could, for example, write our `Energy` module 
type as:

```.cpp
struct Energy : ModuleBase {
    //For simplicity assuming "system_type" is the type of the system
    virtual double run(system_type sys) = 0;
}; 
```

This leads to a bit of boilerplate for defining a module type and we have 
consequentially defined the following convenience macro:

```.cpp
DEFINE_MODULE_TYPE(name, return_value, args...)
```

where the arguments respectively are the name of the resulting class, the 
type returned by the `run` function, and types of the arguments to the `run` 
function.  Using this macro we could have instead defined `Energy` *via*:

```.cpp
DEFINE_MODULE_TYPE(Energy, double, system_type); //Note the required semicolon
```

The use of the macro is strongly recommended to help future-proof your module
types.

By default the module type is not visible to Python.  The header file 
`SDE/PyBindings/PyModule.hpp` contains two functions `register_module` and 
`register_property`, which respectively will register an existing module type
and its corresponding Property specialization with Python.  That said, the 
exact registration requires some conventions to be enforced, which we hide 
behind the macro:
 
```.cpp
DEFINE_PYTHON_PROPERTY(python_module, module_type)
```
 
where the first argument is the `pybind11::module` instance to add the module 
type to and the second argument is the name of the corresponding C++ class.  To
export our `Energy` module type the command is:
 
```.cpp
PYBIND11_MODULE(ExampleModule, m) {
    DEFINE_PYTHON_PROPERTY(m, Energy);
}    
 ```
There's a couple of notes on forming Python bindings:
1. The class type passed as the second argument must be already defined.
2. The class will be accessible from Python via the same name (*i.e.* our 
   `Energy` class would be accessible via `Energy`) 
3. This macro will also expose the corresponding specialization of the 
   Property class to Python (in our case `Property<Energy>`) under the name
   `PropertyEnergy` (or generally speaking whatever name was provided 
   prepended with `Property`).
 
Use of the Python macro is highly recommended also.

@section writing_a_module_main Writing a Module

Once you have selected the module type your module will implement it is time to
write your module.  For the sake of this page we will assume you want to write a
module called `MyEnergyMethod`, which implements the `Energy` module type we 
defined in the previous section.  Generally speaking, in addition to 
implementing the `run` function, you'll also be setting a slew of metadata 
about your module so it is a good idea to declare a non-defaulted default 
constructor which sets your module's defaults.  As an example, the declaration
for our `MyEnergyMethod` class is:

```.cpp
struct MyEnergyMethod : Energy {
    MyEnergyMethod();
    double run(system_type sys) override;
};
```
Per usual C++ practice the declaration of your module will go in a header file.
The actual implementation would then go in a corresponding source file.  Inside
the source file you implement the actual algorithm and set the meta data.  We'll
start with setting the meta data.  Each module has a member `metadata_` that is
a map between recognized meta data properties (*e.g.* name of the module, a
description of the module, or the version) and the value of that datum.  Also
included are members `parameters_` and `submodules_`, which respectively allow
you to set the default parameters and submodules for your module.  The 
`parameters_` member is an object of type Parameters and its usage is thus 
identical to that of other Parameters instances (see 
[here](@ref using_parameters) for example usage).  The following snippet shows
an example of setting the meta data for the module:  


```.cpp
#include "/path/to/header/file.hpp"

MyEnergyMethod::MyEnergyMethod() {
    metadata_[SDE::MetaProperty::name] = "My Energy Method";
    metadata_[SDE::MetaProperty::version] = "1.0.0";
    /* set other meta properties */
    
    submodules_["call back point 1"] = /* get pointer somehow*/
    
}     
```

Given that this is a fictitious module the actual data is a bit sparse, but 
hopefully it conveys the idea.  We start by setting the `metadata_` member.
Here we gave our module the name `"My Energy Method"`, which is what it will 
be called anytime we need to name it (say in an output file) and set its version
to `"1.0.0"`.  There are many additional values that can be set an you are 
encouraged to set as many of them as possible to get the most out of the SDE.

The other important part of our constructor is the setting of the `submodules_`
member.  This member is important as it lists all of the places where your 
module will be calling other modules.  We'll see its usage in a second.  For 
now we simply note that the keys establish the callback points and should be 
more descriptive than what is shown.  Typically the names reflect what the 
module will be computing when it is called (*e.g.* `"Fock Builder"` if it is 
building the Fock matrix).  Presently the names are strings, but the hope is 
that this can eventually be standardized via enumerations or strong types. 
The values are the actual modules to call at those points.  When possible it 
is recommended that a module developer provide a default module; however, it 
is not always possible and a null pointer may be used (signally there is no 
default and the user must set it).

With the constructor out of the way, we turn our attention to the implementation
of the `run` member function.  For our energy module an example implementation
is:
```.cpp
double MyEnergyMethod::run(system_type sys) {
    /* do some work*/
    
    /* pause to call submodule*/
    
    /*we'll use the module under "call back point 1" and assume it's another 
      Energy module*/
    Property<Energy> ei_mod(submodules_.at("call back point 1"));
    auto ei = ei_mod(sys); // call it just like a normal module
    
    /* use ei and do more work*/
    
    return result;
}
```
Like the constructor, the actual details are scare; however, the code does serve
to showcase how to use the `submodules_` member.  As written we have a single
call back point, which we have labeled `"call back point 1"`.  When we get to
that point in the code we call whatever module is registered under the key 
and use it to compute the desired property.

@subsection writing_a_module_python Writing a Module in Python

It is possible to implement a module from Python (assuming the module type has
been exposed to Python).  There are a couple of slight differences relative to
C++.  For concreteness, let's reimplement the `MyEnergyMethod` in Python:

```.py
class MyEnergyMethod(Energy): # Assumes Energy module type has been exported
    def __init__(self):
        Energy.__init__(self) # Good practice to call base's ctor this way
        self._set_submodule("call back point 1", None)
        
    def run(self, sys):
        # Do work
    
        # Call submodule
        ei_mod = PropertyEnergy(self.submodules()["call back point 1"])
        ei = ei_mod(sys)
        
        # use ei
        return result
```

Overall the implementation is largely similar to the C++.  The main difference
is that we have to set the meta data *via* "protected" member functions with 
the prefix `_set_` and we have to retrieve their values via the public 
accessors.  This is an artifact of ensuring the Python bindings don't violate
the C++ modifiers.  The only other minor point is that Python's `None` is used
to denote the lack of a default module since Python doesn't have null pointers. 
