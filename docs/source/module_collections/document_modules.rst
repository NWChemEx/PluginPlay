***********************************
Documenting Modules in a Collection
***********************************

This page shows how to document the modules in your module collection using the
<<<<<<< HEAD
``document_modules`` function in the PluginPlay. This page assumes that you can create
=======
``document_modules`` function in the SDE. This page assumes that you can create
>>>>>>> master
a module collection according to :doc:`./overview` and :doc:`load_modules`.

Generate the Documentation
==========================

This section explains the steps necessary to generate documentation for modules
in a given module collection. To cut straight to the point, see the full
documentation program code below in :ref:`example-module-doc-gen`.

Currently, documentation generation is expected to be performed in its own
binary, which we will refer to as ``build_docs`` here. This program must
<<<<<<< HEAD
include both the ``pluginplay::printing::document_modules`` function and the
=======
include both the ``sde::printing::document_modules`` function and the
>>>>>>> master
``load_modules`` function defined for your modules.

.. literalinclude:: ../../../tests/pluginplay/docs/generate_module_documentation.cpp
   :language: c++
   :lines: 1, 3-5

<<<<<<< HEAD
Next, set up a ``pluginplay::ModuleManager`` and load your modules with the
=======
Next, set up a ``sde::ModuleManager`` and load your modules with the
>>>>>>> master
``load_modules`` you wrote.

.. literalinclude:: ../../../tests/pluginplay/docs/generate_module_documentation.cpp
   :language: c++
   :lines: 17-21

The final step is to generate documentation through
<<<<<<< HEAD
``pluginplay::printing::document_modules``, by providing your ``ModuleManager``
=======
``sde::printing::document_modules``, by providing your ``ModuleManager``
>>>>>>> master
instance (``mm`` here) and the ``std::filesystem::path`` to an existing
directory where the documentation should be output (``doc_path`` here). How this
path is generated is a detail left to the developer's discretion. For simplicity
in the example below, the first command line argument is assumed to be a valid
path, although more validation may be necessary.

.. literalinclude:: ../../../tests/pluginplay/docs/generate_module_documentation.cpp
   :language: c++
   :lines: 23-24

.. _example-module-doc-gen:

Example Program
===============

The following code is a minimal example program generating documentation for
<<<<<<< HEAD
the test modules in PluginPlay. Modify the code according to the TODO comments to
=======
the test modules in SDE. Modify the code according to the TODO comments to
>>>>>>> master
generate documentation for your modules.

.. literalinclude:: ../../../tests/pluginplay/docs/generate_module_documentation.cpp
   :language: c++
   :lines: 1-29
