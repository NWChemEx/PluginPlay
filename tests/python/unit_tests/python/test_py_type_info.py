import pluginplay as pp
import py_test_pluginplay.test_py_test_info as test_pp
import unittest

class TestPyTestInfo(unittest.TestCase):
    def test_has_value(self):
        self.assertFalse(self.defaulted.has_value())
        self.assertTrue(self.wrap_int.has_value())
        self.assertTrue(self.wrap_double.has_value())


    def test_str(self):
        self.assertEqual(str(self.defaulted), '')

        # Actual string is implementation dependent
        self.assertNotEqual(str(self.wrap_int), '')
        self.assertNotEqual(str(self.wrap_double), '')


    def test_hash(self):
        self.assertEqual(hash(self.defaulted), 0)

        # Actual hash is implementation dependent
        self.assertNotEqual(hash(self.wrap_int), 0)
        self.assertNotEqual(hash(self.wrap_double), 0)


    def test_comparisons(self):
        # Default vs. default
        other_default = pp.PyTypeInfo()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Default vs. non-default
        self.assertNotEqual(self.defaulted, self.wrap_int)
        self.assertFalse(self.defaulted == self.wrap_int)

        # Non-default same type
        other_int = test_pp.wraps_int()
        self.assertEqual(self.wrap_int, other_int)
        self.assertFalse(self.wrap_int != other_int)

        # Non-defaults different type
        self.assertNotEqual(self.wrap_int, self.wrap_double)
        self.assertFalse(self.wrap_int == self.wrap_double)


    def test_pass_to_cxx(self):
        self.assertTrue(test_pp.check_int(self.wrap_int))
        self.assertTrue(test_pp.check_double(self.wrap_double))


    def setUp(self):
        self.defaulted = pp.PyTypeInfo()
        self.wrap_int = test_pp.wraps_int()
        self.wrap_double = test_pp.wraps_double()
