Notes on Module I/O Design
==========================

Ultimately our goal is to have the expert API for a module look like:

```
output_map run(input_map inputs);
```

where the return and inputs are maps from strings to opaque objects representing
either the results computed by the module (for `output_map`) or the parameters
the module should use for the computation (`input_map`). As mentioned this is 
the expert API, 99.9% of the time we expect modules to be used *via* a property
type. Such use looks like:

```
std::tuple<output_type1...> run_as<property_type>(input_type1...);
```

While somewhat obfuscated, this is really just a generic function signature.
The returned `std::tuple` allows for multiple returns and the various input 
types correspond to the function parameters. As a more concrete example assume
we have a property type `Area` that computes the area of the shape given to it.
We'll let the return be a double and the input be two `double`s and a string 
(the idea being the string is something like "circle", "square", etc. which 
determines how the two `double`s are interpreted (obviously this API is meant 
only for illustrative purposes)). Subject to these assumptions `run_as` looks
something like:

```
std::tuple<double> run_as<Area>(double param1, double param2, std::string type);
```

Bridging the Gap Between APIs
-----------------------------

The expert API is very generic allowing arbitrary inputs and returns. It does 
this by using type-erasure. The normal API, on the other hand, is a fully 
type-safe API. There's at least two ways to do this. The first way is 
boilerplate heavy, but template light. It requires the user to manually map the
inputs and returns from the type-safe API to the type-erased one. This could be
done in the property type, *e.g.*,

```
//Module's run_as function (just dispatches to the user-defined mapping)
auto run_as<property_type>(Args...args) {
    property_type p;
    return p.run(*this, std::forward<Args...>(args...));
}

//Property type's run function (returning to our Area example)
std::tuple<double> 
run(Module& mod, double param1, double param2, std::string type) {
    auto params = mod.inputs();
    params.change("Dimension 1", param1);
    params.change("Dimension 2", param2);
    params.change("Shape Type", type);
    auto output = mod.run(params);
    return std::make_tuple(output.at<double>("Area"));
}    
```

Note that this becomes very tedious when a module has a lot of inputs/results,
furthermore all of this information needs to appear in other places since the
property type also needs to implement functions for listing the input and output
parameters (this implementation would look nearly identical to the what we
show below and is omitted for the moment). This brings us to the other
alternative, leverage these other functions to automate the normal API to the
type-erased API conversion. This requires a bit of template metaprogramming,
which we'll omit the full details of here, but as far as what a property type 
developer would need to do the result is:

```
class Area : public PropertyType<Area> {

auto inputs() {
    return  add_input<double>("Dimension 1").description("The first dimension")
           .add_input<double>("Dimension 2").description("The second dimension")
           .add_input<std::string>("Shape Type")
           .description("What type of shape is this?");

}

auto results() {
    return add_output<double>("Area").description("The area of the shape.");
}
    
};
```

The idea is that the `add_input`/`add_output` calls creates an object called
`PropertyTypeInputBuilder`/`PropertyTypeOutputBuilder`. These objects build up
a wrapped `input_map`/`output_map` instance containing the inputs/results and
any metadata associated with them. The template metaprogramming aspect comes in,
in the fact that the two builders are templated on the types of each input, for
example the `inputs` function would return something like 
`PropertyTypeInputBuilder<double, double, std::string>`. This tells us that the
arguments to the resulting property type are of types `double`, `double`, and
`std::string`, in that order (the order comes from the order in which 
`add_input` was called). A similar convention holds for the results. With types
that contain the types of the inputs/results, and the order in which they should
appear in the signature, it becomes a straightforward template metaprogramming
exercise to map the actual inputs to their type-erased positions.  

The Opaque Input/Output Types
-----------------------------

While the type-erased inputs and results superficially look a lot a like, 
there's actually a number of differences under the hood, which has led to the
decision to make them two unrelated classes. For posterity those differences
are:

- Checks. It makes sense to check an input before it goes into a module since
  what otherwise may be a valid value might be out of scope for the module. For
  results checks do not make sense since the module should error if it produced 
  a bad value.
- Memory. While both inputs and results are envisioned as read-only rigorously 
  enforced for results), results are usually shared (minimally with the cache).
  The result is that it's more natural to store results in `shared_ptr` 
  instances and take inputs as `const T&` or by value.
- Memoization. Inputs need to be hashed, results do not.
- Defaults. Defaults only make sense for inputs.

Admittedly it is possible to factor some of the code common to the two classes
out, at the moment, this factorization leads to more boilerplate (associated
with making new classes) than it saves. 
    
We run into a similar problem with the builders. There are two situations when
one will trigger a builder: while declaring a property type and when adding
additional inputs/results to a module. Again there are some differences:

- Order. When declaring a property type order of declaration matters, whereas 
  order doesn't matter for a module.
- Module's inputs/results is the union of inputs/results for each satisfied
  property type  
  
In particular it is challenging to both keep track of the order inputs were 
defined in (as part of the class's type) and take unions. One solution is to use
inheritance; however, inheritance doesn't work well with the chaining 
that is part of the builder pattern (you'll slice the derived class off by 
using base class members). For this reason we've chosen to implement two sets
of builders, one for the property types and one for the modules. This also 
unfortunately means that if new metadata is added to the 
ModuleInput/ModuleResult two builders need updated to reflect the changes.
    
  
