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


class MyPlugin(pp.Plugin):
    """A plugin written in Python, set up in __init__."""

    def __init__(self):
        pp.Plugin.__init__(self)
        self.set_name("my_plugin")
        self.set_preferred_providers("provider_a", "provider_b")
        self.add_module("Provider", AProvider())
        self.add_module("Consumer", AConsumer())
        self.change_submod("Consumer", "sub", "Provider")
        self.default_submod_from("provider_a", "Consumer", "sub", "A key")


class TestPlugin(unittest.TestCase):
    def setUp(self):
        self.defaulted = pp.Plugin()
        self.my_plugin = MyPlugin()

    def test_default_state(self):
        self.assertEqual(self.defaulted.name(), "")
        self.assertEqual(self.defaulted.preferred_providers(), [])
        self.assertEqual(self.defaulted.default_submods_from("a"), [])
        self.assertEqual(self.defaulted.get_modules().size(), 0)

    def test_python_plugin(self):
        self.assertEqual(self.my_plugin.name(), "my_plugin")
        self.assertEqual(
            self.my_plugin.preferred_providers(), ["provider_a", "provider_b"]
        )
        modules = self.my_plugin.get_modules()
        self.assertEqual(sorted(modules.keys()), ["Consumer", "Provider"])
        self.assertTrue(modules.has_submod("Consumer", "sub"))

        records = self.my_plugin.default_submods_from("provider_a")
        self.assertEqual(len(records), 1)
        self.assertEqual(records[0].module_key, "Consumer")
        self.assertEqual(records[0].callback_key, "sub")
        self.assertEqual(records[0].provider_key, "A key")
        self.assertEqual(self.my_plugin.default_submods_from("provider_b"), [])

    def test_modules(self):
        self.defaulted.modules().add_module("Provider", AProvider())
        self.assertEqual(self.defaulted.get_modules().count("Provider"), 1)

    def test_set_name_throws(self):
        self.assertRaises(ValueError, self.defaulted.set_name, "")

    def test_set_preferred_providers_throws(self):
        self.assertRaises(
            ValueError, self.defaulted.set_preferred_providers, "a", "a"
        )
        # Can't drop a provider that has recorded defaults
        self.assertRaises(
            ValueError, self.my_plugin.set_preferred_providers, "provider_b"
        )

    def test_default_submod_from_throws(self):
        # Not a preferred provider
        self.assertRaises(
            ValueError,
            self.my_plugin.default_submod_from,
            "provider_c",
            "Consumer",
            "sub",
            "key",
        )
        # Not one of the plugin's modules
        self.assertRaises(
            IndexError,
            self.my_plugin.default_submod_from,
            "provider_a",
            "not a key",
            "sub",
            "key",
        )
        # Not a submodule of the module
        self.assertRaises(
            IndexError,
            self.my_plugin.default_submod_from,
            "provider_a",
            "Consumer",
            "not a callback",
            "key",
        )

    def test_comparisons(self):
        self.assertEqual(self.defaulted, pp.Plugin())
        self.assertNotEqual(self.defaulted, self.my_plugin)
