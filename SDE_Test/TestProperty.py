import unittest
import os
import sys
import SDE

lib_dir = os.path.join(os.path.dirname(os.path.abspath(os.path.curdir)), "lib")
sys.path.append(lib_dir)

import DummyModule

class PyProperty2(DummyModule.TestProperty2):
    def __init__(self):
        DummyModule.TestProperty2.__init__(self)

    def run(self, i):
        return i + 1

class TestProperty(unittest.TestCase):
    def setUp(self):
        self.mod = PyProperty2()
        self.prop = DummyModule.PropertyTestProperty2(self.mod)

    def test_run(self):
        self.assertEqual(self.prop(1), 2)

class TestCPPProperty(unittest.TestCase):
    def setUp(self):
        self.mod = DummyModule.get_cpp_module()
        self.prop = DummyModule.PropertyTestProperty2(self.mod)

    def test_run(self):
        self.assertEqual(self.prop(1), 2)

if __name__ == '__main__':
    unittest.main(verbosity=2)
