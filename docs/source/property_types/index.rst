#########################
Property Type Development
#########################

The topics in this chapter will walk you through writing and designing property 
types.Needing to design a property type is common when a project starts and 
becomes less common when an established set of property types already exists 
(because the likelihood that a property type already exists for your property 
increases).

We continue the electric field example started in the modules chapter, here
focusing on how we designed the ``ElectricField`` property type. For simplicity
we assume that electric fields arise soley from a collection of point charges
and that users only ever want the value of the electric field at a single point.
In a production quality code those are unlikely to be valid assumptions, but
they suffice for our present purposes.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   basics
   advanced