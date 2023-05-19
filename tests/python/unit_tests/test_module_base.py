import pluginplay as pp
import py_test_pluginplay as test_pp
import unittest

class APythonModule(pp.ModuleBase):
    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.satisfies_property_type(test_pp.OneInOneOut())
        self.description("This is a Python Module!!!")
        self.citation("A. Author. The most awesome-est paper ever.")
        self.add_input("An extra input")
        self.add_result("An extra result")
        self.add_submodule(test_pp.OneInOneOut(), "A submodule")


    def run_(self, inputs, submods):
        pt = test_pp.OneInOneOut()
        i0, = pt.unwrap_inputs(inputs)
        i1  = inputs["An extra input"].value()

        da_sum = i0 + i1
        print(da_sum)
        r0 = submods["A submodule"].run_as(pt, da_sum)

        rv = self.results()
        rv = pt.wrap_results(rv, r0)
        rv['An extra result'].change(42)
        return rv


class TestModuleBase(unittest.TestCase):
    def test_results(self):
        self.assertEqual(self.defaulted.results(), {})
        self.assertNotEqual(self.py_mod.results(), {})


    def test_run_as(self):
        self.mm.change_input(self.mod_key, 'An extra input', 2)
        r = self.mm.run_as(test_pp.OneInOneOut(), self.mod_key, 1)
        self.assertEqual(r, 3)


    def setUp(self):
        self.defaulted = pp.ModuleBase()
        self.mm = test_pp.get_mm()
        self.mod_key = 'Python module'
        self.py_mod = APythonModule()
        self.mm.add_module(self.mod_key, self.py_mod)
        submod_key = 'C++ module using every feature'
        self.mm.change_submod(self.mod_key, "A submodule", submod_key)
