@page using_module_loader Using the ModuleLoader Class

This page is aimed primarily at module developers as they are the main consumer
of the ModuleLoader class (aside from the ModuleManager class).  This page 
assumes you have already made a module.  If you have not, head over to 
@ref writing_a_module "Writing a Module" to learn how to write a module.  For
the purposes of this tutorial, we will assume that your module is implemented by
the class `MyAwesomeModule` which is declared in the file `MyAwesomeModule.hpp`.

@section using_ml_definition Defining Your Module's Loader

The simplest way to define a ModuleLoader is via inheritance:  

```.cpp
struct MyAwesomeLoader : public ModuleLoader<MyAwesomeModule> {
    MyAwesomeLoader();
};
```

Here we inherit from the base class `ModuleLoader<MyAwesomeModule>`, although
obviously the template type paramter would be replaced with the name of your 
module.  The only function we need to define is the default constructor.  
Specifically the default constructor should be designed such that the member 
variables of the `ModuleLoaderBase` class are set to the correct details for
your module.  An example implementation is:

```.cpp
#include "MyAwesomeLoader.hpp" // for the declaration
#include "MyAwesomeModule.hpp" // for the actual module

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
    
    sub_modules["stress-energy tensor"] = "";
}
```

This definition really has two parts.  The first part fills in the literal 
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

The second part of the definition is the establishment of the list of 
submodules.  Unlike `ModuleBase::Parameters`, which is set in the constructor of
your module, `ModuleBase::submodules`, needs to be populated with callbacks to
functions.  These are only knowable at runtime.  For this reason, the 
`submodules` member of `ModuleLoaderBase` is of a different type.  More 
specifically, the keys in the `submodules` member establish the various 
call-back points.  The values in the `submodules` member allows the developer
to set a series of default modules by specifying their module key, or, like the 
present example, leave it up to the user to set (implied by leaving the value 
empty).  Obviously in order to set default module keys one has to know what
module keys are in the ModuleManager so default modules are usually only set if
there is an implementation in the same supermodule.
