@page using_modules Using the Module Class Hierarchy

[TOC]

The current page focuses on documentation related to the Module Class Hierarchy,
particularly the ModuleBase, ModuleImpl, and PropertyBase classes.  For the
most part these classes are not geared at the end-user (aside from actually
computing the property).  The majority of their state and member functions 
instead are designed to facilitate interactions between the module and the SDE. 

@section using_modules_acyclic Acyclic Visitor Pattern

The Module class hierarchy relies on the acyclic visitor pattern to ensure that
developers can add additional properties and/or additional algorithms in a 
non-invasive manner.  
