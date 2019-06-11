#TUTORIAL
#
#The purpose of this tutorial is to introduce you to the high-level aspects of
#the SDE framework. This tutorial assumes you have built the SDE (with Python
#bindings) and have installed it in a location visible to your Python
#interpreter (usually by including the install path in your ``PYTHONPATH``
#environment variable).
#
#SDE is designed to be driven from Python; however, as the next tutorial will
#show you, it is entirely possible to drive SDE purely from C++. Anyways, to
#include SDE in your Python script simply do:
from sde import *

#TUTORIAL_START_SKIP
import unittest
import os

dir = os.path.dirname(os.path.realpath(__file__))
dir = os.path.dirname(dir)
cppyy.add_include_path(dir)
cppyy.include("examples/property_type.hpp")
cppyy.include("examples/module_base.hpp")
cppyy.include("examples/workflow.hpp")


class TestWorkflow(unittest.TestCase):
    def test_workflow(self):
        #TUTORIAL_STOP_SKIP

        #TUTORIAL
        #
        #The "App Store" of the SDE is the ModuleManager. The ModuleManager is
        #responsible for storing all of the modules that can be used in a
        #calculation, the default inputs to those modules, and for establishing
        #the call graph. Hence the first task is to populate a ModuleManager
        #instance. For now don't worry about the details and syntax of the
        #``load_modules`` function, just note that you have to load modules
        #before you can use them.
        mm = sde.ModuleManager()
        cppyy.gbl.load_modules(mm)

        #TUTORIAL
        #
        #Generally speaking, each module in the SDE can be used to compute
        #multiple properties. Each property is registered with the SDE using
        #what is called  a "property type". Property types are classes which
        #define the API for computing a property. For example inside the
        #``load_module`` function above we defined a property type ``Area``
        #which takes the dimensions of the shape and returns the area of the
        #shape. We also created a module capable of computing the area of a
        #rectangle and stored it in ``mm`` under the module key "Rectangle"
        #
        #So how do we use the "Rectangle" module to compute the area of a
        #rectange that is say 1.23 by 4.56 (arbitrary units)? Simply by telling
        #``mm`` to run the "Rectangle" module as property type "Area" and
        #providing it the rectangle's dimensions
        area = mm.run_as["Area"]("Rectangle", 1.23, 4.56)

        #TUTORIAL
        #
        #SDE uses Cppyy to generate Python bindings. Unfortunately, Cppyy
        #currently has a bug which makes it unable to automatically deduce
        #template type paramaters when one type parameter is explicitly
        #provided. This is why we had to provide the two "double" strings. Once
        #this bug is fixed the proper syntax will be:
        #
        #.. code-block:: python
        #
        #   area = mm.run_as["Area"]("Prism", 1.23, 4.56)
        #
        #This tutorial should have given you a very high-level overview of what
        #a typical workflow with the SDE looks like. Subsequent tutorials will
        #provide more details on the various aspects of working with and
        #developing for the SDE framework.

        #TUTORIAL_START_SKIP
        self.assertEqual(area, 1.23*4.56)


if __name__ == "__main__":
    unittest.main(verbosity=2)
#TUTORIAL_STOP_SKIP
