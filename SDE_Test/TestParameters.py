import unittest
import os
import sys
import SDE

lib_dir = os.path.join(os.path.dirname(os.path.abspath(os.path.curdir)), "lib")
sys.path.append(lib_dir)

import DummyParameters

class TestParameters(unittest.TestCase):
    def setUp(self):
        self.params = DummyParameters.params

    def test_at(self):
        int_opt = self.params.at("The number 3")
        self.assertEqual(int_opt,3)

        double_opt = self.params.at("Pi")
        self.assertEqual(double_opt,3.1416)

        string_opt = self.params.at("Hello")
        self.assertEqual(string_opt,"Hello world")

        vector_opt = self.params.at("A vector")
        self.assertEqual(vector_opt,[1,2,3])

    def test_change(self):
        self.params.change("The number 3", 2)
        int_opt = self.params.at("The number 3")
        self.assertEqual(int_opt,2)

        self.params.change("Pi",1.57)
        double_opt = self.params.at("Pi")
        self.assertEqual(double_opt,1.57)

        self.params.change("Hello", "Sup world")
        string_opt = self.params.at("Hello")
        self.assertEqual(string_opt,"Sup world")

        self.params.change("A vector", [3, 4, 5])
        vector_opt = self.params.at("A vector")
        self.assertEqual(vector_opt,[3,4,5])

    def test_badchange(self):
        self.assertRaises(ValueError, self.params.change,
                          "The number 3", -1)
        self.assertRaises(RuntimeError, self.params.change,
                          "The number 3", 3.14)

    def test_description(self):
        self.assertEqual(self.params.get_description("The number 3"), "some description")

    def test_traits_track_changes(self):
        self.params.change("The number 3", 2)
        self.assertFalse(SDE.OptionTraits.non_default in self.params.get_traits("The number 3"))
        self.params.track_changes()
        self.params.change("The number 3", 1)
        self.assertTrue(SDE.OptionTraits.non_default in self.params.get_traits("The number 3"))

    def test_count(self):
        self.assertTrue("The number 3" in self.params)
        self.assertFalse("Not here" in self.params)

if __name__ == '__main__':
    unittest.main(verbosity=2)