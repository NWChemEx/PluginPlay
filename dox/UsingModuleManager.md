@page using_mm Using the ModuleManager Class

The ModuleManager class is responsible for keeping track of all of the modules
known to the SDE.  It also is responsible for managing the lifetimes of the 
objects that create the modules (the so-called "loaders").  The current page is
designed to introduce the ModuleManager class, particularly its usage

@section using_mm_examples Example Usage of the ModuleManager Class

By default a ModuleManager instance has no modules.  Thus the first thing a user
needs to do is add modules.  Adding modules is slightly complicated in that we
need to shield the ModuleManager from the module's actual type to prevent 
coupling.  This is done via an opaque function called a module loader or 
"loader" for short.  All loaders have the same signature:

```.cpp
std::shared_ptr<SDE::ModuleBase> loader_name(); 
```

The ModuleManager actually stores loaders and uses them to generate a module 
when needed.  Generally speaking, loaders will be provided by the libraries you 
are using (and also typically automatically loaded into the ModuleManager for 
you).  Nevertheless it is straightforward (although tedious for a large set of
modules) to manually add a loader:

```.cpp
ModuleMangaer mm;
mm.insert("key", my_loader);
```

This example tells the ModuleManager instance `mm` about our hypothetical 
loader, `my_loader`.  Furthermore, it associates this loader with the module
key `"key"`.  This means any time a user requests a module via the key 
`"key"` they will get one created *via* the loader `my_loader`.  Obtaining a 
module is done via `ModuleManager::at`:

```.cpp
auto mod = mm.at("key"); //Retrieves the module we just added
```

Once a user has a module they are free to inspect it or modify its parameters
(assuming it is not locked) as described on the 
[Using Modules](@ref using_modules) page.  If the module has been locked, but
the user still wants to call it with modified parameters they must make a copy
of the module.  This is done *via* `ModuleManager::duplicate`:

```.cpp
auto new_key = mm.duplicate("key");
``` 

This call will make a copy of the module stored under the key `"key"` that 
contains the same parameters, but is not locked.  The resulting copy is stored
under the returned key.  Finally, it is possible to inquire into whether or not
a particular key has been utilized:

```.cpp
auto is_used = mm.count("key");
```


@subsection using_mm_python Using ModuleManager From Python

For the most part the ModuleManager works the same from Python as it does 
from C++.

```.py
mm = ModuleManager() # Make a new manager

# Somehow fill mm (See below for important considerations)

if "a key" in mm: #Check if the ModuleManager contains a module with key "a key"
    mod = mm.at("a key") #it does, get it
    # do stuff with mod

new_key = mm.duplicate("a key") # Duplicate the module with key "a key"
```

As the example hints at, there is a very important consideration to keep in 
mind wheny loading modules from Python.  Specifically, let us assume there is a
property interface `Energy`, and that we implemented a module in Python.  For
concreteness our implementation is:

```.py
class MyEnergy(Energy):
    def run(self, system):
        return -0.5 # *cross fingers and hope system is a single hydrogen atom*
```  

Attempting to create a loader, like we did in C++, won't work:

```.py
def my_loader():
    return MyEnergy()
    
mm.insert("My Energy Module", my_loader)
prop = PropertyEnergy(mm.at("My Energy Module"))
prop(sys) # <---Runtime error attempting to call pure virtual method run    
```

What is happening is that the resulting `MyEnergy` class has two parts the C++ 
part (the `Energy` class and its base class `ModuleBase`) and a Python part 
(`MyEnergy`).  When `MyEnergy` is passed to C++ it's by `ModuleBase`.  Long 
story short, because the instance was created in Python and passed to C++ by the
base class, there's no way for C++ to tell Python to increment the reference 
count for the pure Python part (debatedly this is a bug in Pybind11 and may 
be fixed at some point by 
[this PR](https://github.com/pybind/pybind11/issues/1145)).  
In turn, Python sees the Python part go in to the function and come back 
untouched, thus concluding it is safe to delete `MyEnergy` and resultantly 
slicing the class.

The solution is you have to create a functor:
```.python
class MyEnergyLoader:
    def __init__(self):
        self.mods = []
    def __call__(self):
        self.mods.append(MyEnergy())
        return self.mods[-1]
        
mm.insert("My Energy Module", MyEnergyLoader())                
```
This works because the functor keeps one reference to the module within it 
(which is tied to the lifetime of the functor) and the ModuleManager keeps the
functor alive.




