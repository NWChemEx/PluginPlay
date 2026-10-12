# Copyright 2026 NWChemEx-Project
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

import pluginplay as pp
import py_test_pluginplay as test_pp


class AProvider(pp.ModuleBase):
    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.satisfies_property_type(test_pp.OneInOneOut())

    def run_(self, inputs, submods):
        pt = test_pp.OneInOneOut()
        (i0,) = pt.unwrap_inputs(inputs)
        return pt.wrap_results(self.results(), i0)


class AConsumer(pp.ModuleBase):
    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.satisfies_property_type(test_pp.OneInOneOut())
        self.add_submodule(test_pp.OneInOneOut(), "sub")

    def run_(self, inputs, submods):
        pt = test_pp.OneInOneOut()
        (i0,) = pt.unwrap_inputs(inputs)
        return pt.wrap_results(self.results(), i0)


class TestModuleMap(unittest.TestCase):
    def setUp(self):
        self.defaulted = pp.ModuleMap()
        self.has_mods = pp.ModuleMap()
        self.has_mods.add_module("Provider", AProvider())
        self.has_mods.add_module("Consumer", AConsumer())

    def test_default_ctor(self):
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(len(self.defaulted), 0)
        self.assertTrue(self.defaulted.empty())
        self.assertEqual(self.defaulted.keys(), [])

    def test_add_module(self):
        self.defaulted.add_module("Provider", AProvider())
        self.assertEqual(self.defaulted.count("Provider"), 1)
        self.assertEqual(self.defaulted.at("Provider").get_name(), "Provider")

        # Keys are unique (and case-insensitive)
        self.assertRaises(
            ValueError, self.has_mods.add_module, "provider", AProvider()
        )

    def test_count(self):
        self.assertEqual(self.has_mods.count("Provider"), 1)
        self.assertEqual(self.has_mods.count("not a key"), 0)

    def test_at(self):
        self.assertEqual(self.has_mods.at("Consumer").get_name(), "Consumer")
        self.assertRaises(IndexError, self.has_mods.at, "not a key")

    def test_keys(self):
        self.assertEqual(
            sorted(self.has_mods.keys()), ["Consumer", "Provider"]
        )

    def test_change_submod_by_key(self):
        self.assertFalse(self.has_mods.has_submod("Consumer", "sub"))
        self.has_mods.change_submod("Consumer", "sub", "Provider")
        self.assertTrue(self.has_mods.has_submod("Consumer", "sub"))

    def test_change_submod_throws(self):
        self.assertRaises(
            IndexError,
            self.has_mods.change_submod,
            "not a key",
            "sub",
            "Provider",
        )
        self.assertRaises(
            IndexError,
            self.has_mods.change_submod,
            "Consumer",
            "not a callback",
            "Provider",
        )

    def test_default_submod(self):
        self.assertTrue(
            self.has_mods.default_submod("Consumer", "sub", "Provider")
        )
        self.assertFalse(
            self.has_mods.default_submod("Consumer", "sub", "Provider")
        )

    def test_comparisons(self):
        self.assertEqual(self.defaulted, pp.ModuleMap())
        self.assertNotEqual(self.defaulted, self.has_mods)
