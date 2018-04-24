@page parallelization Parallelization Within the SDE

This page focuses on the considerations concerning parallelization within the
SDE.

@section parallelization_multithread Multithread Parallelization

We will require that all functions meant to be utilized from the SDE API are
thread-safe.  What this means is that all functions are guaranteed to work
correctly if they are invoked by multiple threads.  This does not guarantee that
the result of calling a function from multiple threads is independent of thread
order.  

(Fill in rest later)

@section parallelization_multiprocess Multiprocess Parallelization

@subsection parallelization_mp_overview Overview of Multiprocess Parallelization

The SDE utilizes an SPMD parallelization model.  More specifically when the end
user creates an SDE instance, `S`, they provide `S` a set of parallel resources.
Of particular importance for this section is that this set includes a set of 
processes (this set of processes need not be the set associated with 
`mpi_comm_world`).  When a module, `M`, is created using `S` all processes 
associated with `S` are given to `M`.  `M` is then free to use those processes
to do parallel work or to ignore them all together and execute serially.  
Regardless of `M`'s decision all results computed by `M` are expected to be 
available to all processes associated with `S`.  This does not mean that
the results need to be replicated, it simply means that each process must have a
handle to the result and that the handle's API does not depend on which 
process is accessing the data.  Importantly most distributed tensor libraries
will work, as is, given these restrictions.

This entire SPMD model can be done recursively too.  Specifically, it is 
possible to split `S` into two or more new SDE instances.  Each of these new
instances contains a disjoint set of resources, such that the union of the 
resources is the same as those available to `S`.  Using the new SDE instances 
proceeds identically to workflow described in the above paragraph.  When the
user is finished with the new SDE instances their Caches must be synched to 
`S`'s Cache.  This splitting procedure is envisioned as ensuring parallel 
resources do not get wasted. 

@subsection parallelization_mp_synchronization Synchronization

For the most part the description in the previous section is pretty 
straightforward and non-restrictive.  Arguably the only restrictive requirement
is that the results be synchronized.  The purpose of this subsection is to 
describe that restriction in more detail.  

Generally speaking there are two synchronization points in the SDE: when an SDE 
instance is created/destroyed.  In creating an SDE instance, `B`, from an 
existing SDE instance, `A`, `B`'s Cache must be populated with `A`'s contents in  
such a manner that the data within `B`'s Cache lives purely on the parallel
resources accessible to `B`.  In destroying `B` we must ensure the opposite, 
namely that all of `B`'s Cache entries are accessible from `A`'s parallel 
resources.  The first synchronization is a no-op for any entry in `A`'s Cache
that is replicated.  On the other-hand the second synchronization is in general
always an operation so as to ensure that the set difference of `A` and `B`'s 
parallel resources is made aware of the additional results.

There are numerous complications associated with these synchronization points, 
in an attempt to do these synchronizations in an opaque manner we define the 
Synchronizer class:

```.cpp
template<typename T>
struct Synchronizer {
    virtual void operator()(const SDE& old, const SDE& new, T& value) {
        if(old < new) //replicate value on remaining processes
        else if(new < old) // do nothing
        else //do nothing
    }
};
```

Users are free to specialize this class for a particular type.  This class will
need specifically specialized for types that store their state in a distributed
fashion.  The Synchronizer class is a brute-force solution that allows the SDE 
to (possibly) move a module's entire Cache.  Given that parallelization in the
SDE is envisioned as being coarse-grained, synchronization has the 
possibility of being expensive.  Thus a module must specifically request 
synchronization by specifying the `distributed` trait.  This trait is not 
applied recursively by default.  For the most part it is assumed that Caches
only contain a few entries, such that moving the entire Cache is not too much
additional overhead.  If need be, a mechanism for only moving select entries can
be implemented.
