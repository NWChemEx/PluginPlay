import os
import sys
import importlib

"""This script will generate the module api documentation for a plugin library.

Usage
-----

::

   usage: generate_module_dox.py library_name

   positional arguments:
     library_name  The name of the library for which the documentation is 
                   supposed to be generated.

This script checks that only one library name is passed and that it can import
pluginplay and the library specified. If the library is a plugin, a module
manager is constructed and the modules of the library are loaded into it.
The document_modules function is called on the manager.
"""

if __name__ == "__main__":
    # Exit if more than a single argument is passed
    if len(sys.argv) != 2:
        sys.exit()
    target = sys.argv[1].replace("_cxx_api", "")

    # Add paths to libraries
    cwd = os.getcwd()
    build_dir = os.path.join(cwd, "build")
    pluginplay_dir = os.path.join(cwd, "build", "_deps", "pluginplay-build")
    sys.path.append(build_dir)
    sys.path.append(pluginplay_dir)

    # Exit on any exception
    try:
        pluginplay = importlib.import_module("pluginplay")
        library = importlib.import_module(target)
        # If this library is a plugin, try to print the dox.
        if hasattr(library, "load_modules"):
            mm = pluginplay.ModuleManager()
            library.load_modules(mm)
            # This is no-op if docs/source/module_api doesn't exist
            pluginplay.document_modules(mm, "docs/source/module_api")
    except Exception as e:
        print(f"An exception caught: {e}")
        sys.exit()

