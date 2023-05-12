import pluginplay as pp
import py_test_pluginplay.test_python_wrapper as test_pp
import unittest

class TestPythonWrapper(unittest.TestCase):
    def test_has_value(self):
        self.assertTrue(self.list.has_value())
        self.assertTrue(self.dict.has_value())


    def test_unwrap(self):
        self.assertTrue(test_pp.cxx_unwrap_list(self.list))
        self.assertTrue(test_pp.cxx_unwrap_dict(self.dict))


    def test_comparisons(self):
        # Wrap the same list
        other_list = pp.PythonWrapper([1, 2, 3])
        self.assertEqual(self.list, other_list)
        self.assertFalse(self.list != other_list)

        # Wrap different lists
        diff_list = pp.PythonWrapper([2, 3, 4])
        self.assertNotEqual(self.list, diff_list)
        self.assertFalse(self.list == diff_list)

        # Wrap the same dictionary
        other_dict = pp.PythonWrapper({"hello" : 42, "world" : 123})
        self.assertEqual(self.dict, other_dict)
        self.assertFalse(self.dict != other_dict)

        # Wrap different dictionaries
        diff_dict = pp.PythonWrapper({"hello" : 123, "world" : 42})
        self.assertNotEqual(self.dict, diff_dict)
        self.assertFalse(self.dict == diff_dict)

        # Wrap entirely different types
        self.assertNotEqual(self.list, self.dict)
        self.assertFalse(self.list == self.dict)


    def setUp(self):
        self.list = pp.PythonWrapper([1, 2, 3])
        a_dict = {"hello" : 42, "world" : 123}
        self.dict = pp.PythonWrapper(a_dict)
