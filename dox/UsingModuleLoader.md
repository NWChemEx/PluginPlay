@page using_module_loader Using the ModuleLoader Class

This page is aimed primarily at module developers as they are the main consumer
of the ModuleLoader class (aside from the ModuleManager class).  This page 
assumes you have already made a module.  If you have not, head over to 
@ref writing_a_module "Writing a Module" to learn how to write a module.  For
the purposes of this tutorial, we will assume that your module is implemented by
the class `MyAwesomeModule` which is declared in the file `MyAwesomeModule.hpp`.

@section using_ml_declare Declaring Your Module's Loader

The ModuleLoader class is designed so that it is implemented by inheritance. To
that end we'll go ahead and define a class `MyAwesomeLoader` in a header 
file `MyAwesomeLoader.hpp`.  A typical declaration looks like:
```.cpp
#include <SDE/ModuleLoader.hpp> // needed so we can implement a module loader
class MyAwesomeLoader : public SDE::ModuleLoader {
public:
    MyAwesomeLoader();
    
    module_pointer get_module() const override;
}; // class MyAwesomeLoader
```

Ultimately the only field that your derived class must contain is the 
`get_module` member function (it's abstract in ModuleLoader and thus the 
compiler will complain if you don't implement it).  Declaring a default 
constructor for your loader is optional.  What really matters is that somehow
the fields of the ModuleLoader base class get populated.  It is conventional to
do this in the default constructor, but it is ultimately up to the module 
developer to decide how to do this.  

Assuming one follows convention, then declaration of a module's loader is a lot 
of boiler plate.  For this reason we have defined the macro 
`SDE_DECLARE_MODULE_LOADER` which will automatically declare a module loader for
you.  More specifically the code snippet above is identical to:

```.cpp
#include <SDE/ModuleLoader.hpp> // needed so we can implement a module loader
SDE_DECLARE_MODULE_LOADER(MyAwesomeLoader); // class MyAwesomeLoader
```

@section using_ml_definition Defining Your Module's Loader

Now that we have the declaration of the loader done, let us turn our attention 
to defining it.  Conventionally this is done in a source file; however, it is 
possible to also do this directly in the header file if desired.  We'll assume
that the following is contained in a source file `MyAwesomeLoader.cpp`. First
let us implement the `get_module` function:

```.cpp
#include "MyAwesomeLoader.hpp" // for the declaration
#include "MyAwesomeModule.hpp" // for the actual module

// Convenience typedef of the pointer returned by get_module
using module_pointer = typename MyAwesomeLoader::module_pointer;

module_pointer MyAwesomeLoader::get_module() const {
    return std::make_unique(new MyAwesomeModule);
}
```

The definition is pretty straightforward we declare a new `unique_ptr` filled
with a default constructed instance of our module.  There's no requirement that
your module needs to be default constructed; however, no information will be
passed into the `get_module` function when the `ModuleManager` creates an 
instance of your module.  This in turn severely limits the sources for 
additional input to your module's constructor.

With the `get_module` function defined all that remains is to fill in the data
fields regarding your module.  As mentioned, the conventional way to do this is 
in the constructor for your module's loader.  Continuing from our last code
snippet:

```.cpp
using MetaProperty = SDE::MetaProperty; // bring MetaProperty into scope
MyAwesomeLoader::MyAwesomeLoader() {
    meta_data[MetaProperty::name] = "My Awesome Test Module";
    meta_data[MetaProperty::version] = "1.2.3";
    meta_data[MetaProperty::authors] = "Albert Einstein";
    meta_data[MetaProperty::description] = 
      "Computes the Einstein field equations.";
    meta_data[MetaProperty::citations] = 
      "Einstein, Albert (1916). The Foundation of the General Theory of "
      "Relativity. *Annalen der Physik*. 354 (7): 769";
    
    params["cosmological constant"] = Option{0.0, "Lambda in the EFE"};                                             
    
    sub_modules["stress-energy tensor"] = "";
}
```

This definition really has three parts.  The first part fills in the literal 
meta-data pertaining to the module.  Of particular importance in this meta-data
is the version of the module.  As modules will be used to compute scientific 
results it is important that the exact state of that module's source code be 
recorded.  It also is important that the module developer increment the version
everytime a change occurs (change being anything from changing a default value 
to modifying the source code).  The "author" and "citations" meta-data fields
are primarily there to ensure proper credit is given to the author(s) of the 
module as well as the discoverer of any relevant theoretical developments 
utilized by the module.  Finally the "name" and "description" fields enable 
descriptive logging and documentation.  

The second part of the definition is the establishment of the default parameters
inherent to the algorithm.  Here we are defining a single parameter, the 
cosmological constant (this is primarily an illustrative example as typically 
one would get constants from the SDE, although there are some scientific 
reasons why one may want to allow the cosmological constant to vary, but 
that's digressing).  `params` is ultimately a full-fledged instance of the 
Parameters class and developers are free to use all of that class's bells and
whistles to define the default set.  See 
@ref using_paramaters "Using the Parameters Class" for more details on how to
use the Parameters class.

Finally we set the list of submodules used by our module.  Generally speaking
modules are envisioned as **not** being monolithic entities.  Instead each 
module should be designed to do one task (here computing the metric tensor) and
defer to other modules to compute other tasks (here computing the stress-energy 
tensor).  This is done by establishing call-back points.  Each call-back point
is a place where a module will call another module.  The keys in the 
`submodules` member establish the various call-back points.  The values in the
`submodules` member allows the developer to set a series of default modules or,
like the present example, leave it up to the user to set (implied by leaving 
the value empty).
