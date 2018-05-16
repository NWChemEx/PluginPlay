import os
import sys
import unittest
# These next couple lines add ../lib to the PythonPath
parent_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
sys.path.append(os.path.join(parent_dir, "lib"))
import SDE
import DummyModule

# This fixture tests the member functions of ModuleBase
class TestModuleBase(unittest.TestCase):
    def test_default_ctor(self):
        mod = SDE.ModuleBase()
        self.assertIsNotNone(mod)

# This class implements a DummyModule by inheriting from the C++ type
class PyDummyModule(DummyModule.DummyModuleAPI):
    def __init__(self):
        DummyModule.DummyModuleAPI.__init__(self)
    def run_(self, an_integer):
        return an_integer == 1

# This fixture tests the class we just implemented
class TestDummyModule(unittest.TestCase):
    def test_default_ctor(self):
        mod = PyDummyModule()
        self.assertIsNotNone(mod)
    def test_call_function(self):
        mod = PyDummyModule()
        self.assertTrue(mod(1))
        self.assertFalse(mod(2))


# This fixture tests calling modules through Python wrapped properties
class TestProperty(unittest.TestCase):
    def test_python_mod_with_property(self):
        mod = PyDummyModule()
        prop = DummyModule.DummyModuleProp(mod)
        self.assertTrue(prop(1))
        self.assertTrue(prop(2))
    def test_cpp_mod_with_property(self):
        prop = DummyModule.DummyModuleProp(DummyModule.get_module())
        self.assertTrue(prop(1))
        self.assertFalse(prop(2))

if __name__ == '__main__':
    unittest.main(verbosity=2)
