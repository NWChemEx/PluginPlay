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
import py_test_pluginplay.test_any_field as test_pp
import unittest


class TestAnyField(unittest.TestCase):

    def test_reset(self):
        self.assertFalse(self.defaulted.has_value())
        self.defaulted.reset()
        self.assertFalse(self.defaulted.has_value())

        self.assertTrue(self.has_vector.has_value())
        self.has_vector.reset()
        self.assertFalse(self.has_vector.has_value())

        self.assertTrue(self.has_list.has_value())
        self.has_list.reset()
        self.assertFalse(self.has_list.has_value())

    def test_comparisons(self):
        # Two default instances
        other_default = pp.any.AnyField()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # One default, one not default
        self.assertNotEqual(self.defaulted, self.has_vector)
        self.assertFalse(self.defaulted == self.has_vector)

        # Both have same vector
        other_vector = test_pp.get_vector()
        self.assertEqual(self.has_vector, other_vector)
        self.assertFalse(self.has_vector != other_vector)

        # Same lists
        other_list = pp.make_any_field([1, 2, 3])
        self.assertEqual(self.has_list, other_list)
        self.assertFalse(self.has_list != other_list)

        # Different lists
        diff_list = pp.make_any_field([2, 3, 4])
        self.assertNotEqual(self.has_list, diff_list)
        self.assertFalse(self.has_list == diff_list)

        # Different types (note that the vector was specifically chosen so that
        # it is convertible to the list)
        self.assertNotEqual(self.has_list, self.has_vector)
        self.assertFalse(self.has_list == self.has_vector)

    def test_has_value(self):
        self.assertFalse(self.defaulted.has_value())
        self.assertTrue(self.has_vector.has_value())
        self.assertTrue(self.has_list.has_value())

    def test_owns_value(self):
        self.assertFalse(self.defaulted.owns_value())
        self.assertTrue(self.has_vector.owns_value())
        self.assertTrue(self.has_list.owns_value())

    def setUp(self):
        self.defaulted = pp.any.AnyField()
        self.has_vector = test_pp.get_vector()
        self.has_list = pp.make_any_field([1, 2, 3])
