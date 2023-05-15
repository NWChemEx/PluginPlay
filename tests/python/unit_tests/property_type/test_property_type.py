import pluginplay as pp
import py_test_pluginplay as test_pp
import py_test_pluginplay.test_property_type as test_pt
import unittest

class TestPropertyType(unittest.TestCase):
    def test_ctor(self):
        self.assertNotEqual(self.null_pt, None)
        self.assertNotEqual(self.one_in, None)


    def test_unwrap_inputs(self):
        # Expects one integer
        inp = test_pt.one_in_inputs()
        i0, = self.one_in.unwrap_inputs(inp)
        self.assertEqual(i0, 1)

        # Expects a double and an integer
        inp = test_pt.two_in_inputs()
        i0, i1 = self.two_in.unwrap_inputs(inp)
        self.assertEqual(i0, 3.14)
        self.assertEqual(i1, 1)

        # Expects a string, a double, and an integer
        inp = test_pt.three_in_inputs()
        i0, i1, i2 = self.three_in.unwrap_inputs(inp)
        self.assertEqual(i0, "Hello World!")
        self.assertEqual(i1, 3.14)
        self.assertEqual(i2, 1)


    def test_wrap_results(self):
        outs = self.one_out.wrap_results(test_pt.result_map_one(), 1)
        self.assertEqual(outs["Result 1"].value(), 1)

        outs = self.two_out.wrap_results(test_pt.result_map_two(), 1, 'c')
        self.assertEqual(outs["Result 1"].value(), 1)
        self.assertEqual(outs["Result 2"].value(), 'c')


    def setUp(self):
        self.null_pt      = test_pp.NullPT()
        self.one_in       = test_pp.OneIn()
        self.two_in       = test_pp.TwoIn()
        self.three_in     = test_pp.ThreeIn()
        self.defaulted_in = test_pp.OptionalInput()
        self.poly_input   = test_pp.PolymorphicOptions()
        self.one_out      = test_pp.OneOut()
        self.two_out      = test_pp.TwoOut()
