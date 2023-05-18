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
import py_test_pluginplay.test_module_input as test_pp
import unittest

class TestModuleInput(unittest.TestCase):
    def test_has_type(self):
        self.assertFalse(self.defaulted.has_type())
        self.assertTrue(self.ifloat.has_type())
        self.assertTrue(self.ilist.has_type())
        self.assertTrue(self.ilist2.has_type())


    def test_has_value(self):
        self.assertFalse(self.defaulted.has_value())
        self.assertFalse(self.ifloat.has_value())
        self.assertFalse(self.ilist.has_value())
        self.assertTrue(self.ilist2.has_value())


    def test_has_description(self):
        self.assertFalse(self.defaulted.has_description())
        self.assertFalse(self.ifloat.has_description())
        self.assertFalse(self.ilist.has_description())
        self.assertFalse(self.ilist2.has_description())


    def test_is_optional(self):
        self.assertFalse(self.defaulted.is_optional())
        self.assertFalse(self.ifloat.is_optional())
        self.assertFalse(self.ilist.is_optional())
        self.assertFalse(self.ilist2.is_optional())


    def test_is_transparent(self):
        self.assertFalse(self.defaulted.is_transparent())
        self.assertFalse(self.ifloat.is_transparent())
        self.assertFalse(self.ilist.is_transparent())
        self.assertFalse(self.ilist2.is_transparent())


    def test_ready(self):
        self.assertFalse(self.defaulted.ready())
        self.assertFalse(self.ifloat.ready())
        self.assertFalse(self.ilist.ready())
        self.assertTrue(self.ilist2.ready())


    def test_is_valid(self):
        # No type should throw
        self.assertRaises(Exception, self.defaulted.is_valid, "hi")

        # Can set an integer with an integer
        self.assertTrue(self.ifloat.is_valid(1))

        # Can set an integer with a double
        self.assertTrue(self.ifloat.is_valid(1.1))

        # Can't set an integer with a string
        self.assertFalse(self.ifloat.is_valid("hi"))

        # Can set a list of integers with a list of integers
        self.assertTrue(self.ilist.is_valid([1, 2, 3]))

        # Can set a list of integers with a tuple of integers
        self.assertTrue(self.ilist.is_valid((1, 2, 3)))

        # Can't set a list of integers with a list of strings
        self.assertFalse(self.ilist.is_valid(["hi"]))

        # Can't set a list of integers with a string
        self.assertFalse(self.ilist.is_valid("hi"))


    def test_change(self):
        self.assertRaises(Exception, self.defaulted.change, "hi")

        # Sanity check
        self.assertFalse(self.ifloat.has_value())

        # Can change the value
        x = self.ifloat.change(1)
        self.assertTrue(self.ifloat.has_value())
        self.assertEqual(self.ifloat.value(), 1)

        # Returns an alias to self.ifloat
        x.change(2)
        self.assertTrue(self.ifloat.value(), 2)


    def test_set_default(self):
        self.assertRaises(Exception, self.defaulted.set_default, "hi")

        # Sanity check
        self.assertFalse(self.ilist.has_value())

        x = self.ilist.set_default([1, 2, 3])
        self.assertTrue(self.ilist.has_value())
        self.assertEqual(self.ilist.value(), [1, 2, 3])


    def test_set_description(self):
        self.assertFalse(self.defaulted.has_description())

        x = self.defaulted.set_description("Hello World!")

        # Changed the value of self.defaulted
        self.assertTrue(self.defaulted.has_description())
        self.assertEqual(self.defaulted.description(), "Hello World!")

        # x is alias of self.defaulted
        x.set_description("foo bar")
        self.assertEqual(self.defaulted.description(), "foo bar")


    def test_make_optional(self):
        self.assertFalse(self.defaulted.is_optional())

        x = self.defaulted.make_optional()

        # Changed value of self.defaulted
        self.assertTrue(self.defaulted.is_optional())

        # x is an alias of self.defaulted
        x.make_required()
        self.assertFalse(self.defaulted.is_optional())


    def test_make_required(self):
        self.defaulted.make_optional()
        self.assertTrue(self.defaulted.is_optional())

        x = self.defaulted.make_required()

        # Changed value of self.defaulted
        self.assertFalse(self.defaulted.is_optional())

        # x is an alias of self.defaulted
        x.make_optional()
        self.assertTrue(self.defaulted.is_optional())


    def test_make_opaque(self):
        self.defaulted.make_transparent()
        self.assertTrue(self.defaulted.is_transparent())

        x = self.defaulted.make_opaque()

        # Changed the value
        self.assertFalse(self.defaulted.is_transparent())

        # x is an alias of self.defaulted
        x.make_transparent()
        self.assertTrue(self.defaulted.is_transparent())


    def test_make_transparent(self):
        self.assertFalse(self.defaulted.is_transparent())

        x = self.defaulted.make_transparent()

        # Changed the value
        self.assertTrue(self.defaulted.is_transparent())

        # x is an alias of self.defaulted
        x.make_opaque()
        self.assertFalse(self.defaulted.is_transparent())


    def test_value(self):
        self.assertRaises(Exception, self.defaulted.value)
        self.assertRaises(Exception, self.ifloat.value)
        self.assertRaises(Exception, self.ilist.value)

        # This ensures Python can play with C++ values
        self.assertEqual(self.ilist2.value(), [1, 2, 3])


    def test_description(self):
        self.assertRaises(Exception, self.defaulted.description)

        self.ifloat.set_description("Hello World")
        self.assertEqual(self.ifloat.description(), "Hello World")


    def test_check_descriptions(self):
        self.assertEqual(self.defaulted.check_descriptions(), set())


    def test_comparisons(self):
        # Default vs default
        other_default = pp.ModuleInput()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Default vs non-default
        self.assertNotEqual(self.defaulted, self.ifloat)
        self.assertFalse(self.defaulted == self.ifloat)

        # Both have same type
        other_ifloat = test_pp.get_i_float()
        self.assertEqual(self.ifloat, other_ifloat)
        self.assertFalse(self.ifloat != other_ifloat)

        # Different types
        self.assertNotEqual(self.ifloat, self.ilist)
        self.assertFalse(self.ifloat == self.ilist)

        # Different value-ness
        self.assertNotEqual(self.ilist, self.ilist2)
        self.assertFalse(self.ilist == self.ilist2)

        # Different values
        other_ilist = test_pp.get_i_list()
        other_ilist.change([2, 3, 4])
        self.assertNotEqual(self.ilist2, other_ilist)
        self.assertFalse(self.ilist2 == other_ilist)

        # Different description-ness
        other_default.set_description("Hello World!")
        self.assertNotEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted == other_default)

        # Different optional-ness
        other_ifloat.make_optional()
        self.assertNotEqual(self.ifloat, other_ifloat)
        self.assertFalse(self.ifloat == other_ifloat)

        # Different transparent-ness
        other_default2 = pp.ModuleInput()
        other_default2.make_transparent()
        self.assertNotEqual(self.defaulted, other_default2)
        self.assertFalse(self.defaulted == other_default2)


    def test_cxx_can_unwrap_py_objects(self):
        self.ilist.change([1, 2, 3])
        self.assertTrue(test_pp.unwrap_python_list(self.ilist))


    def setUp(self):
        self.defaulted = pp.ModuleInput()
        self.ifloat = test_pp.get_i_float()
        self.ilist  = test_pp.get_i_list()
        self.ilist2 = test_pp.get_filled_i_list()
