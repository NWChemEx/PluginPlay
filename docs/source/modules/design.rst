******************
Designing a Module
******************

The previous pages covered how to actually code up a module. They did not 
provide any guidance on how to design your module.

Additional Inputs
=================

The property type APIs are necessarilly general and rigid. This can pose 
difficulties when your module needs additional input. The SDE provides you
with two options for obtaining the extra input your module needs:

#. Define a module-specific input parameter
#. Call a submodule

Module-Specific Input
---------------------

Many algorithms have algorithmic parameters associated with them such as
thresholds, tolerances, maximum number of iterations, **etc.**. Generally 
speaking it is best to expose these parameters to the user and not hardcode
them. These parameters are typically not exposed through the property type and
thus can not be set by the caller as positional arguments. At the same time it
is usually the end-user, not the calling function, which wants to manipulate
these parameters, **e.g.** it is the end-user which has decided that a 
particularly tight energy convergence criteria is needed not the function which
is requesting the energy. For these types of parameters it makes sense to make
them module-specific inputs.

In our ``ScreenedCoulombsLaw`` example (TODO: Add link) we used a threshold to
determine when point charges should be screened out. This threshold is really an
algorithmic detail. Once it has been set (either to the default value or by the
end-user) it generally won't be touched again. The other option, having this 
threshold be set by a submodule doesn't really make a whole lot of sense because
there really isn't an algorithm that can compute it (typically the default value 
is determined by extensive benchmarking which establishes what value produces a
tolerable error; while that process could be encapsulated in a module, onece 
it's been done you might as well just use the value).

Call a Submodule
----------------

Module-specific inputs are primarily for algorithmic parameters. Submodules are
used primarily to establish data dependencies among computed properties. For 
example in our ``ClassicalForce`` tutorial (TODO: add link) we used a submodule 
to establish that when we compute the force on the particle it will depend on 
the electric field (which in general must also be computed).  We did this by
calling a submodule that satisfied the ``ElectricField`` property type. By using
a submodule instead of a hard-coded call our ``ClassicalForce`` module works
with both the ``CoulombsLaw`` and ``ScreenedCoulombsLaw`` submodules (as well as
any other modules users choose to write which satisfy the ``ElectricField``
property type).

Using a module-specific input, for the electric field, in the ``ClassicalForce``
module doesn't make a lot of sense because: the value of the electric field is
system dependent (needs to be computed for every system) and the end-user 
typically doesn't have the value sitting around (unless they've already called 
a module satisfying the ``ElectricField`` property type).

