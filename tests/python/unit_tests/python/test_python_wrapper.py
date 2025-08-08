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

import unittest

import py_test_pluginplay.test_python_wrapper as test_pp

import pluginplay as pp


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
        other_dict = pp.PythonWrapper({"hello": 42, "world": 123})
        self.assertEqual(self.dict, other_dict)
        self.assertFalse(self.dict != other_dict)

        # Wrap different dictionaries
        diff_dict = pp.PythonWrapper({"hello": 123, "world": 42})
        self.assertNotEqual(self.dict, diff_dict)
        self.assertFalse(self.dict == diff_dict)

        # Wrap entirely different types
        self.assertNotEqual(self.list, self.dict)
        self.assertFalse(self.list == self.dict)

    def setUp(self):
        self.list = pp.PythonWrapper([1, 2, 3])
        a_dict = {"hello": 42, "world": 123}
        self.dict = pp.PythonWrapper(a_dict)
