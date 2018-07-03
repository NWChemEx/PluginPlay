import unittest
import SDE.SDE as SDE
import py_sde_utils as PySDEUtils

# This set of tests mirrors the C++ TestModule.cpp file as closely as possible
# to ensure that behavior between C++ and Python is identical.
#
# With only C++ there was only one implementation language and usage language.
# With the pair of C++ and Python we have four possibilities:
#
# 1. C++ module used in C++
#    - Tested in unit test: TestModule.cpp
# 2. Py module used in Py
#    - This unit test, members: missing_submodule, not_ready_submodule
# 3. C++ module used from Python
#    - This unit test: use_cxx_module
# 4. Python module used from C++
#    - This unit test: use_python_module


# Module with missing submodule
class PyProperty1(PySDEUtils.TestProperty):
    def __init__(self):
        PySDEUtils.TestProperty.__init__(self)
        self._set_submodule("Prop1", None)
        self._set_metadata(SDE.MetaProperty.name, "Prop1")

    def run(self, x):
        return x + 1

# Instance of said submodule
prop1 = PyProperty1()

# Module with not ready submodule
class PyProperty2(PySDEUtils.TestProperty):
    def __init__(self):
        PySDEUtils.TestProperty.__init__(self)
        self._set_submodule("Prop1", prop1)

    def run(self, x):
        return x + 1

# Module that is r2g
class PyProperty3(PySDEUtils.TestProperty):
    def __init__(self):
        PySDEUtils.TestProperty.__init__(self)

    def run(self, x):
        return x + 1

#Instance of said submodule
prop3 = PyProperty3()

# This fixture tests the member functions of ModuleBase
class TestModuleBase(unittest.TestCase):
    def check_module(self, proptype, mod, metadata, submods):
        self.assertEqual(mod.submodules(), submods)
        self.assertEqual(mod.metadata(), metadata)
        self.assertFalse(mod.locked())
        self.assertEqual(len(mod.not_ready()), 1)
        mod.change_submodule("Prop1", prop3)
        submods["Prop1"] = prop3
        self.assertEqual(mod.submodules(), submods)
        mod.lock()
        self.assertTrue(mod.locked())
        self.assertRaises(RuntimeError, mod.change_submodule, "Prop1", prop1)
        self.assertEqual(mod.run_as(proptype, 2), 3)

    def setUp(self):
        self.prop = PySDEUtils.TestProperty

    def test_missing_submodule(self):
        mod = PyProperty1()
        md = {SDE.MetaProperty.name: "Prop1"}
        self.check_module(self.prop, mod, md, {"Prop1" : None})

    def test_not_ready_submodule(self):
        mod = PyProperty2()
        smods = {"Prop1": prop1}
        self.check_module(self.prop, mod, {}, smods)

    def test_use_cxx_module(self):
        mod = PySDEUtils.get_cpp_module()
        self.check_module(self.prop, mod, {}, {"Prop1" : None})

    def test_use_python_module(self):
        mod = PyProperty3()
        self.assertTrue(PySDEUtils.run_py_mod(mod))

if __name__ == '__main__':
    unittest.main(verbosity=2)
