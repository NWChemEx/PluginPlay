import pluginplay as pp
import py_test_pluginplay as test_pp
import py_test_pluginplay.test_property_type as test_pt
import unittest

class TestPropertyType(unittest.TestCase):
    def test_ctor(self):
        self.assertNotEqual(self.null_pt, None)
        self.assertNotEqual(self.one_in, None)
        self.assertNotEqual(self.two_in, None)
        self.assertNotEqual(self.three_in, None)
        self.assertNotEqual(self.defaulted_in, None)
        self.assertNotEqual(self.poly_input, None)
        self.assertNotEqual(self.one_out, None)
        self.assertNotEqual(self.two_out, None)


    def test_type(self):
        corr = test_pt.get_types()
        self.assertEqual(self.null_pt.type(), corr["NullPT"])
        self.assertEqual(self.one_in.type(), corr["OneIn"])
        self.assertEqual(self.two_in.type(), corr["TwoIn"])
        self.assertEqual(self.three_in.type(), corr["ThreeIn"])
        self.assertEqual(self.defaulted_in.type(), corr["OptionalInput"])
        self.assertEqual(self.poly_input.type(), corr["PolymorphicOptions"])
        self.assertEqual(self.one_out.type(), corr["OneOut"])
        self.assertEqual(self.two_out.type(), corr["TwoOut"])


    def test_inputs(self):
        inps = self.null_pt.inputs()
        self.assertEqual(inps, {})

        inps = self.one_in.inputs()
        corr = test_pt.one_in_inputs(False)
        self.assertEqual(inps, corr)

        inps = self.two_in.inputs()
        corr = test_pt.two_in_inputs(False)
        self.assertEqual(inps, corr)

        inps = self.three_in.inputs()
        corr = test_pt.three_in_inputs(False)
        self.assertEqual(inps, corr)


    def test_results(self):
        rvs = self.null_pt.results()
        self.assertEqual(rvs, {})

        rvs = self.one_out.results()
        corr = test_pt.result_map_one()
        self.assertEqual(rvs, corr)

        rvs = self.two_out.results()
        corr = test_pt.result_map_two()
        self.assertEqual(rvs, corr)


    def test_unwrap_inputs(self):
        # Expects one integer
        inp = test_pt.one_in_inputs(True)
        i0, = self.one_in.unwrap_inputs(inp)
        self.assertEqual(i0, 1)

        #N.B. Because ThreeIn inherits from TwoIn which inherits from OneIn the
        #     order the inputs are unwrapped is reverse what the input keys
        #     suggest, i.e, Option 3 is unwrapped first, then 2, then 1.

        # Expects a double and an integer
        inp = test_pt.two_in_inputs(True)
        i0, i1 = self.two_in.unwrap_inputs(inp)
        self.assertEqual(i0, 3.14)
        self.assertEqual(i1, 1)

        # Expects a string, a double, and an integer
        inp = test_pt.three_in_inputs(True)
        i0, i1, i2 = self.three_in.unwrap_inputs(inp)
        self.assertEqual(i0, "Hello World!")
        self.assertEqual(i1, 3.14)
        self.assertEqual(i2, 1)


    def test_wrap_inputs(self):
        new_inp = self.one_in.wrap_inputs(test_pt.one_in_inputs(True), 42)
        self.assertEqual(new_inp["Option 1"].value(), 42)

        #N.B. Because ThreeIn inherits from TwoIn which inherits from OneIn the
        #     order the inputs are wrapped in reverse what the input keys
        #     suggest, i.e, Option 3 is wrapped first, then 2, then 1.

        new_inp = self.two_in.wrap_inputs(test_pt.two_in_inputs(True), 1.23, 42)
        self.assertEqual(new_inp["Option 1"].value(), 42)
        self.assertEqual(new_inp["Option 2"].value(), 1.23)

        inp = test_pt.three_in_inputs(True)
        new_inp = self.three_in.wrap_inputs(inp, "foo", 1.23, 42)
        self.assertEqual(new_inp["Option 1"].value(), 42)
        self.assertEqual(new_inp["Option 2"].value(), 1.23)
        self.assertEqual(new_inp["Option 3"].value(), "foo")


    def test_wrap_results(self):
        outs = self.one_out.wrap_results(test_pt.result_map_one(), 1)
        self.assertEqual(outs["Result 1"].value(), 1)

        outs = self.two_out.wrap_results(test_pt.result_map_two(), 1, 'c')
        self.assertEqual(outs["Result 1"].value(), 1)
        self.assertEqual(outs["Result 2"].value(), 'c')


    def test_unwrap_results(self):
        outs = self.one_out.wrap_results(test_pt.result_map_one(), 1)
        i0, = self.one_out.unwrap_results(outs)
        self.assertEqual(i0, 1)

        outs = self.two_out.wrap_results(test_pt.result_map_two(), 1, 'c')
        i0, i1 = self.two_out.unwrap_results(outs)
        self.assertEqual(i0, 1)
        self.assertEqual(i1, 'c')


    def setUp(self):
        self.null_pt      = test_pp.NullPT()
        self.one_in       = test_pp.OneIn()
        self.two_in       = test_pp.TwoIn()
        self.three_in     = test_pp.ThreeIn()
        self.defaulted_in = test_pp.OptionalInput()
        self.poly_input   = test_pp.PolymorphicOptions()
        self.one_out      = test_pp.OneOut()
        self.two_out      = test_pp.TwoOut()
