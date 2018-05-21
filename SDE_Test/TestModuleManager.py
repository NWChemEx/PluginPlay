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

def test_mm(tester, mm, keys):
    tester.assertRaises(ValueError, mm.insert, "", tester.loader)
    tester.assertRaises(IndexError, mm.at, "Not an actual key")
    tester.assertFalse("Not an actual key" in mm)
    for k in keys:
        tester.assertTrue(k in mm)
        tester.assertRaises(ValueError, mm.insert, k, tester.loader)
        mod = mm.at(k)
        prop = DummyModule.PropertyTestProperty2(mod)
        tester.assertEqual(prop(1), 2)
        new_key = mm.duplicate(k)
        tester.assertTrue(new_key in mm)
        tester.assertRaises(ValueError, mm.duplicate, k, new_key)

def make_module():
    mod = PyProperty2()
    return mod

class TestModuleManager(unittest.TestCase):
    def setUp(self):
        self.mm = SDE.ModuleManager()
        self.mod = PyProperty2()
        self.key = "a key"
        self.loader = make_module

    def test_default_ctor(self):
        test_mm(self, self.mm, [])

    def test_filled_cpp_module(self):
        self.mm.insert(self.key, DummyModule.get_cpp_module)
        test_mm(self, self.mm, [self.key])

    def test_filled_py_module(self):
        self.mm.insert(self.key, self.loader)
        test_mm(self, self.mm, [self.key])


if __name__ == '__main__':
    unittest.main(verbosity=2)
