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

Going back to our electric field example we've been using. If we want to screen