import unittest
import os
import sys
import SDE

lib_dir = os.path.join(os.path.dirname(os.path.abspath(os.path.curdir)), "lib")
sys.path.append(lib_dir)

import DummyModule

class PyProperty2(DummyModule.TestProperty2API):
    def run(self, i):
        return i + 1

# This fixture tests the member functions of ModuleBase
class TestModuleBase(unittest.TestCase):
    def test_property2(self):
        mod = PyProperty2()
        self.assertEqual(mod.run(1), 2)

class TestPropertyBase(unittest.TestCase):
    def test_from_py(self):
        mod = PyProperty2()
        prop = DummyModule.TestProperty2(mod)
        self.assertEqual(prop(1), 2)

if __name__ == '__main__':
    unittest.main(verbosity=2)
