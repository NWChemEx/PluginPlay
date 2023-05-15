# Copyright 2023 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import pluginplay as pp
import py_test_pluginplay.test_module_result as test_pp
import unittest

class TestModuleResult(unittest.TestCase):
    def test_has_type(self):
        self.assertFalse(self.defaulted.has_type())
        self.assertTrue(self.rfloat.has_type())
        self.assertTrue(self.rlist.has_type())
        self.assertTrue(self.rlist2.has_type())


    def test_has_value(self):
        self.assertFalse(self.defaulted.has_value())
        self.assertFalse(self.rfloat.has_value())
        self.assertFalse(self.rlist.has_value())
        self.assertTrue(self.rlist2.has_value())


    def test_has_description(self):
        self.assertFalse(self.defaulted.has_description())
        self.assertFalse(self.rfloat.has_description())
        self.assertFalse(self.rlist.has_description())
        self.assertFalse(self.rlist2.has_description())


    def test_change(self):
        self.assertRaises(Exception, self.defaulted.change, "hi")

        # Sanity check
        self.assertFalse(self.rfloat.has_value())

        # Can change the value
        self.rfloat.change(1)
        self.assertTrue(self.rfloat.has_value())
        self.assertEqual(self.rfloat.value(), 1)


    def test_set_description(self):
        self.assertFalse(self.defaulted.has_description())

        x = self.defaulted.set_description("Hello World!")

        # Changed the value of self.defaulted
        self.assertTrue(self.defaulted.has_description())
        self.assertEqual(self.defaulted.description(), "Hello World!")

        # x is alias of self.defaulted
        x.set_description("foo bar")
        self.assertEqual(self.defaulted.description(), "foo bar")


    def test_value(self):
        self.assertRaises(Exception, self.defaulted.value)
        self.assertRaises(Exception, self.rfloat.value)
        self.assertRaises(Exception, self.rlist.value)

        # This ensures Python can play with C++ values
        self.assertEqual(self.rlist2.value(), [1, 2, 3])


    def test_description(self):
        self.assertRaises(Exception, self.defaulted.description)

        self.rfloat.set_description("Hello World")
        self.assertEqual(self.rfloat.description(), "Hello World")


    def test_comparisons(self):
        # Default vs default
        other_default = pp.ModuleInput()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Default vs non-default
        self.assertNotEqual(self.defaulted, self.rfloat)
        self.assertFalse(self.defaulted == self.rfloat)

        # Both have same type
        other_rfloat = test_pp.get_r_float()
        self.assertEqual(self.rfloat, other_rfloat)
        self.assertFalse(self.rfloat != other_rfloat)

        # Different types
        self.assertNotEqual(self.rfloat, self.rlist)
        self.assertFalse(self.rfloat == self.rlist)

        # Different value-ness
        self.assertNotEqual(self.rlist, self.rlist2)
        self.assertFalse(self.rlist == self.rlist2)

        # Different values
        other_rlist = test_pp.get_r_list()
        other_rlist.change([2, 3, 4])
        self.assertNotEqual(self.rlist2, other_rlist)
        self.assertFalse(self.rlist2 == other_rlist)

        # Different description-ness
        other_default.set_description("Hello World!")
        self.assertNotEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted == other_default)


    def test_cxx_can_unwrap_py_objects(self):
        self.rlist.change([1, 2, 3])
        self.assertTrue(test_pp.unwrap_python_list(self.rlist))


    def setUp(self):
        self.defaulted = pp.ModuleInput()
        self.rfloat = test_pp.get_r_float()
        self.rlist  = test_pp.get_r_list()
        self.rlist2 = test_pp.get_filled_r_list()
