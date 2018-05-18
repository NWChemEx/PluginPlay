import unittest
import os
import sys

lib_dir = os.path.join(os.path.dirname(os.path.abspath(os.path.curdir)), "lib")
sys.path.append(lib_dir)

import DummyModule

class PyProperty2(DummyModule.TestProperty2API):
    def __init__(self):
        DummyModule.TestProperty2API.__init__(self)

    def run(self, i):
        return i + 1

# This fixture tests the member functions of ModuleBase
class TestModuleBase(unittest.TestCase):
    def setUp(self):
        self.mod = PyProperty2()
        self.corr_submods = {}
        self.corr_metadata = {}
        #self.corr_params = SDE.Parameters

    def test_run(self):
        self.assertEqual(self.mod.run(1), 2)

    def test_submodules(self):
        submods = self.mod.submodules()
        self.assertEqual(submods, self.corr_submods)

    def test_submodules_not_alias(self):
        submods = self.mod.submodules()
        submods["prop2"] = self.mod
        self.assertEqual(self.mod.submodules(), self.corr_submods)

    def test_metadata(self):
        md = self.mod.metadata()
        self.assertEqual(md, self.corr_metadata)

    def test_metadata_not_alias(self):
        md = self.mod.metadata()
        md["prop2"] = self.mod
        self.assertEqual(self.mod.metadata(), self.corr_submods)

    #def test_parameters(self):
    #     params = self.mod.parameters()
    #     self.assertEqual(params, self.corr_parameters)
    #
    #def test_parameters_not_alias(self):
    #     params = self.mod.parameters()
    #     params.cange("prop2", <value>)
    #     #Check it's a deep-copy
    #     self.assertEqual(self.mod.params(), self.corr_parameters)

    def test_lock(self):
        self.assertFalse(self.mod.locked())
        self.mod.lock()
        self.assertTrue(self.mod.locked())

    def test_change_submodule_locked(self):
        self.mod.lock()
        self.assertRaises(RuntimeError, self.mod.change_submodule, "Prop2",
                          self.mod)

    def test_change_submodule_invalid_key(self):
        self.assertRaises(IndexError, self.mod.change_submodule, "prop 4",
                          self.mod)


if __name__ == '__main__':
    unittest.main(verbosity=2)
