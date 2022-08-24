.. _python_basics:

############################
Basics of Writing a PyModule
############################

.. note::

   PluginPlay allows you to write modules in Python and/or in C++. This
   page details the Python procedure. See :ref:`cpp_basics` for the C++
   procedure.

.. warning::

   Writing modules in Python is relatively new. These instructions detail
   the current procedure and are subject to change as infrastructure and
   best practices evolve.

To avoid confusion, PluginPlay modules written in Python are called "PyModules"
and not Python modules. The latter terminology is already widespread in the
Python community and refers to a file containing Python code. Like the
C++ tutorial we assume the setup described in :ref:`e_and_m_101`. We
additionally assume that Python bindings exist for the ``ElectricField``
, ``Point``, and ``PointCharge`` classes and that these bindings
live in the ``pluginplay_examples`` Python package.



Unlike writing a C++ module, it is not necessary to declare a PyModule
in advance.
