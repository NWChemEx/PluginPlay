import unittest
import SDE.SDE as SDE
import py_sde_utils as PySDEUtils

class PyProperty2(PySDEUtils.TestProperty):
    def __init__(self):
        PySDEUtils.TestProperty.__init__(self)

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
        if mod.not_ready():
            mod.change_submodule("Prop1", PyProperty2())
        tester.assertEqual(mod.run_as(PySDEUtils.TestProperty, 1), 2)
        new_key = mm.duplicate(k)
        tester.assertTrue(new_key in mm)
        tester.assertRaises(ValueError, mm.duplicate, k, new_key)

class pymake_module:
    def __init__(self):
        self.mods = []
    def __call__(self):
        self.mods.append(PyProperty2())
        return self.mods[-1]

def cpp_make_module():
    return PySDEUtils.get_cpp_module()

class TestModuleManager(unittest.TestCase):
    def setUp(self):
        self.mm = SDE.ModuleManager()
        self.mod = PyProperty2()
        self.key = "a key"
        self.loader = pymake_module()

    def test_default_ctor(self):
        test_mm(self, self.mm, [])

    def test_filled_cpp_module(self):
        self.mm.insert(self.key, PySDEUtils.get_cpp_module)
        test_mm(self, self.mm, [self.key])

    def test_filled_cpp_from_py(self):
        self.mm.insert(self.key, cpp_make_module)
        test_mm(self, self.mm, [self.key])

    def test_filled_py_module(self):
        self.mm.insert(self.key, self.loader)
        test_mm(self, self.mm, [self.key])

if __name__ == '__main__':
    unittest.main(verbosity=2)
