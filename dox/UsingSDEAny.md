@page using_sdeany Using the SDEAny Class

[TOC]

This page is meant to be a resource for developers wanting to understand how to 
use the SDEAny class.  In particular it discusses some of the computer science
behind the class and presents some illustrative examples.

@section type_erasure Type Erasure

This section describes the technique of type-erasure in a general sense.  The
next section focuses on the application of this technique to the SDEAny class.

@subsection te_problem The Problem

Say we have a class meant to store some data.  One may try something like:

```.cpp
struct MyDataStore {
    std::vector<double> data;
};
```
This works great if all of the data you're trying to store is of type `double`.
If you also want to store, say, integers you'd have to do something like:

```.cpp
struct MyDataStore {
    std::vector<double> fdata;
    std::vector<int> idata;
};
``` 
(or use the fact that integers can be written as floats).  For storing arbitrary
data types, this solution is clearly not sustainable as it requires you to have
a member for each type you want to store.

@subsection te_solution The Solution

In C the solution is to use a `void *`.  In C++, we can do much better by using
the type-erasure pattern.  The pattern works by defining a base class, which 
we'll call `TEBase` (short for type-erasure base).  `TEBase` is basically a 
place-holder so an empty class (with a virtual destructor to make it 
polymorphic) suffices for now:

```.cpp
struct TEBase {
    virtual ~TEBase() {}
};
``` 

From `TEBase` we derive the class that will hold the actual value.  We'll call
this class `TE`.  Since `TE` will hold an object of arbitrary type it must be
templated on that type.  `TE` can be defined something like:

```.cpp
template<typename T>
struct TE : TEBase {
    T value;
};
```

That's the entire pattern. To see it in action we can store both an integer and
a double in an `std::vector`:

```.cpp
std::vector<TEBase> data;

data.push_back(TE<double>{3.14}); //Works b/c TE<double> inherits from TEBase
data.push_back(TE<int>{5}); //Works b/c TE<int> inherits from TEBase

// double the_double = data[0].value; //doesn't work TEBase has no member value
// int the_int = data[1].value; //doesn't work TEBase has no member value

// To get the value back, first get the wrapper
TEBase& wrapped_double = data[0];

// Next, downcast to the derived type 
TE<double>& double_holder = dynamic_cast<TE<double>&>(wrapped_double);

// Finally, access the "value" member:
double the_double = double_holder.value;
``` 

The entire thing works because we're passing the wrapped instance around via the
opaque base class `TEBase`.  Note that unlike the C solution, this solution is 
type-safe (`dynamic_cast<T>` will throw `std::bad_cast` if a particular `TEBase`
instance was not created as a `TE<T>`).

@section te_sde_any Type-Erasure Applied to SDEAny

The main considerations behind the design of the SDEAny class were discussed at 
length on the [SDEAny API](@ref sdeany_api) page.  This section mainly focuses 
on the additional type-erasure considerations specific to the SDEAny class that
were not covered in the previous section.

The simple `TEBase`/`TE` class hierarchy introduced in the previous section does
nothing besides hold the instance.  Furthermore, how it holds the instance is a
bit clunky.  Given the prevalence of type-erasure in generic C++ code a common
solution is to use `boost::any` (`std::any` in C++17 compliant code).  At its
core these classes work a lot like `TEBase`/`TE` except they provide a much 
nicer API.  However, these classes are inadequate for our needs as we want to
be able to manipulate the wrapped instance, in generic ways, without 
downcasting.  For example, we'll want to be able to serialize it.  Assuming 
`TEBase` was instead defined like:

```.cpp
struct TEBase {
    virtual ~TEBase(){}
    
    virtual void load(Archiver& ar) = 0; 
    virtual void save(Archiver& ar)const =0;
};    
```
We could implement the `load/save` functions in the derived class like:

```.cpp
template<typename T>
struct TE : TEBase {
    T value;
    
    void load(Archiver& ar) override {
        ar(value); 
    }
    
    void save(Archiver& ar) const override {
        ar(value);
     }        
};
```

Now when we call `TE::save` or `TE::load` the wrapped instance is serialized for
us without us having to downcast.  As should be evident, we can't add this 
functionality to `boost::any` or `std::any` without modifying the source code so
that the base class contains the necessary virtual functions.  This is why we
choose to implement SDEAny ourselves.

@section sdeany_examples Example Usage
