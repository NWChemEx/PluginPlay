import SDE.SDE as SDE
import py_sde_utils as PySDEUtils
import unittest

"""
Note: Right now it's not possible to create and fill new Option or Parameters
classes from Python.  Thus the specification for what options a module supports
must be done from C++.  Consequentially, we only need to test accessing and
modifying C++ instances of the Option and Parameters classes from Python. 
"""


class TestOption(unittest.TestCase):
    def check_option(self, opt, val, desc, traits):
        self.assertEqual(opt.get(), val)
        self.assertEqual(opt.description, desc)
        self.assertEqual(opt.traits, traits)
        if val == 3:
            self.assertTrue(opt == PySDEUtils.get_option())
        else:
            self.assertTrue(opt != PySDEUtils.get_option())

    def setUp(self):
        self.val = 3
        self.desc = "Any positive number"
        self.traits = {SDE.OptionTraits.optional,
                       SDE.OptionTraits.transparent}

    def test_cxx_option(self):
        opt = PySDEUtils.get_option()
        self.check_option(opt, self.val, self.desc, self.traits)
        self.assertTrue(opt.is_valid(4))
        self.assertFalse(opt.is_valid(-1))
        opt.change(4)
        self.check_option(opt, 4, self.desc, self.traits)
        self.assertRaises(ValueError, opt.change, -1)



class TestParameters(unittest.TestCase):
    def check_params(self, params, opts):
        self.assertEqual(len(params), len(opts))
        nrk = "Not a real key"
        self.assertFalse(nrk in params)
        self.assertRaises(IndexError, params.change, nrk, 2)
        self.assertRaises(IndexError, params.get_option, nrk)
        self.assertRaises(IndexError, params.get_value, nrk)
        for k,v in opts.items():
            self.assertTrue(k in params)
            self.assertTrue(v == params.get_option(k))
            self.assertTrue(v.description == params.get_description(k))
            self.assertTrue(v.traits == params.get_traits(k))
            self.assertEqual(v.get(), params.get_value(k))

    def setUp(self):
        self.key = "The number 3"
        self.non_default = SDE.OptionTraits.non_default

    def test_cxx_parameters(self):
        params, opts = PySDEUtils.get_params()
        self.check_params(params, opts)
        self.assertRaises(ValueError, params.change, self.key, -1)
        ps, opts = PySDEUtils.get_params()
        self.assertTrue(params == ps)
        params.change(self.key, 4)
        self.assertTrue(params != ps)
        params.track_changes()
        params.change(self.key, 3)
        self.assertTrue(self.non_default in params.get_traits(self.key))
        params.change("Hello World", "Goodbye World")
        self.assertTrue(self.non_default in params.get_traits("Hello World"))

if __name__ == '__main__':
    unittest.main(verbosity=2)
