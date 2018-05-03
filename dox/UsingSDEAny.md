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
downcasting.  For example, we'll want to be able to hash it.  Assuming 
`TEBase` was instead defined like:

```.cpp
struct TEBase {
    virtual ~TEBase(){}
    
    virtual void hash(Hasher& h) const = 0; 
};    
```
We could implement the `hash` functions in the derived class like:

```.cpp
template<typename T>
struct TE : TEBase {
    T value;
    
    void hash(Hasher& h) override {
        h(value); 
    }
     
};
```

Now when we call `TE::hash` the wrapped instance is hashed for us without us 
having to downcast.  As should be evident, we can't add this functionality to 
`boost::any` or `std::any` without modifying the source code so that the base
class contains the necessary virtual functions.  This is why we choose to 
implement SDEAny ourselves.

@section sdeany_examples Example Usage

This section is designed to provide example usage of the SDEAny class.  Keep in
mind that the SDEAny class is considered a detail of the SDE library and as such
will not be used by anyone aside from developers working within the SDE.

By far and away, the number one use case for the SDEAny class is type-erasure.
This is straightforward:

```.cpp
// Make some data to erase the type of:
double x=3.14;
std::vector<int> v{1, 2, 3, 4, 5};

SDEAny wrapped_double(x); // erases the type of a copy of the double
SDEAny wrapped_copy_v(v); // erases the type of a copy of v
SDEAny wrapped_v(std::move(v)); //erases the type of v
```

In the previous example we first made the instance we wanted to wrap and then
either copied or moved it into the SDEAny instance.  We also can directly make
the instance inside the SDEAny:

```.cpp
SDEAny wrapped_double; //Declare an empty SDEAny

//Fill construct the instance inside the SDEAny
// N.B. the constructed instance is returned.
double& value = wrapped_double.emplace<double>(3.14);

//For convenience can use the make_SDEAny wrapper to do this in one step
//this one wraps a default constructed std::vector<int> instance
auto wrapped_v = make_SDEAny<std::vector<int>>();

// The above make_SDEAny call in a slightly cleaner fashion to show syntax
using vector_t = std::vector<int>;
auto wrapped_v2 = make_SDEAny<vector_t>();
```

The opposite of the type-erasure use case is retrieval, *i.e.* retrieving the
type-erased instance in a type-safe manner.  Again, we have tried to make this
API as simple as possible:

```.cpp
//Assuming the same wrapped_v instance from the previous example
auto& the_vector = SDEAnyCast<std::vector<int>>(wrapped_v);

//Assuming the wrapped double from the previous example
auto& the_double = SDEAnyCast<double>(wrapped_double);

//N.B. this is a type-safe operation, e.g. this will throw std::bad_cast
try {
    auto& the_string = SDEAnyCast<std::string>(wrapped_double);
}
catch(const std::bad_cast& e) {
    // handle exception
}
```

The final use case for the SDEAny class is interacting with the wrapped instance
in an opaque manner.  One such example is for the Parameters class, where we 
will need to hash the wrapped options for memoization.

```.cpp
// Assume this vector of SDEAny instances was created elsewhere
std::vector<SDEAny> te_instances;

Hasher h(HashType::Hash128); // our hasher instance

// Loop over elements and add them to the hasher
for(auto& x : te_instances)
    h(x);

auto hv = h.finalize(); // get the resulting hash

// Since BPHash knows how to hash std::vector, could have just done:
h(te_instances);
``` 

