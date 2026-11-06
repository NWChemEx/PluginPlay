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

# The call graph the drivers configure is "Wrapper" -> "Adder" -> "Leaf" (a)
# and "Other leaf" (b). "Adder" returns a + b + "offset", the leaves return
# their "x", so "Wrapper" returns 1 + 1 + 0 = 2 by default.


class Leaf(pp.ModuleBase):
    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.satisfies_property_type(test_pp.OneOut())
        self.add_input("x").set_default(1)

    def run_(self, inputs, submods):
        pt = test_pp.OneOut()
        return pt.wrap_results(self.results(), inputs["x"].value())


class Adder(pp.ModuleBase):
    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.satisfies_property_type(test_pp.OneOut())
        self.add_submodule(test_pp.OneOut(), "a")
        self.add_submodule(test_pp.OneOut(), "b")
        self.add_input("offset").set_default(0)

    def run_(self, inputs, submods):
        pt = test_pp.OneOut()
        a = submods["a"].run_as(pt)
        b = submods["b"].run_as(pt)
        offset = inputs["offset"].value()
        return pt.wrap_results(self.results(), a + b + offset)


class Wrapper(pp.ModuleBase):
    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.satisfies_property_type(test_pp.OneOut())
        self.add_submodule(test_pp.OneOut(), "inner")

    def run_(self, inputs, submods):
        pt = test_pp.OneOut()
        value = submods["inner"].run_as(pt)
        return pt.wrap_results(self.results(), value)


def run_root(driver, submods):
    """The drivers below return the value of their "root" submodule"""
    pt = test_pp.OneOut()
    value = submods["root"].run_as(pt)
    return pt.wrap_results(driver.results(), value)


class XDriver(pp.DriverBase):
    """Forwards its "x" to "Leaf"'s "x" """

    def __init__(self):
        pp.DriverBase.__init__(self)
        self.satisfies_property_type(test_pp.OneOut())
        self.add_submodule(test_pp.OneOut(), "root").set_default("Wrapper")
        self.add_input("x").set_description("x of Leaf").set_default(
            1
        ).set_target("Leaf", "x")

    def run_(self, inputs, submods):
        return run_root(self, submods)


class ChainDriver(pp.DriverBase):
    """Chooses "Adder"'s "b" and "offset" with its "mode" """

    def __init__(self):
        pp.DriverBase.__init__(self)
        self.satisfies_property_type(test_pp.OneOut())
        self.add_submodule(test_pp.OneOut(), "root").set_default("Wrapper")
        self.add_input("mode").set_default("plain")
        self.add_input("offset").set_default(0)

        self.when("mode", "plain").set_submod(
            "Adder", "b", "Other leaf"
        ).otherwise("mode", "double").set_submod(
            "Adder", "b", "Leaf"
        ).forward_input(
            "Adder", "offset", "offset"
        ).otherwise().set_input(
            "Adder", "offset", 100
        )
        self.wire_memoization(False, ["Leaf"])

    def run_(self, inputs, submods):
        return run_root(self, submods)


class TestDriverBase(unittest.TestCase):
    def setUp(self):
        self.mm = pp.ModuleManager()
        self.mm.add_module("Leaf", Leaf())
        self.mm.add_module("Other leaf", Leaf())
        self.mm.add_module("Adder", Adder())
        self.mm.change_submod("Adder", "a", "Leaf")
        self.mm.change_submod("Adder", "b", "Other leaf")
        self.mm.add_module("Wrapper", Wrapper())
        self.mm.change_submod("Wrapper", "inner", "Adder")
        self.mm.add_module("X driver", XDriver())
        self.mm.add_module("Chain driver", ChainDriver())
        self.pt = test_pp.OneOut()

    def run_as(self, key):
        return self.mm.run_as(self.pt, key)

    def test_developer_request(self):
        request = XDriver().call_graph_request()
        self.assertFalse(request.empty())

        requests = request.requests()
        self.assertEqual(len(requests), 1)
        self.assertEqual(
            requests[0].kind(), pp.WiringRequest.kind_type.forward_input
        )
        self.assertEqual(requests[0].source_key(), "x")
        self.assertEqual(requests[0].target_key(), "x")
        self.assertEqual(str(requests[0].module_key()), "Leaf")
        self.assertEqual(
            requests[0].to_string(),
            'forwarding input "x" to "Leaf"\'s input "x"',
        )
        self.assertEqual(str(request.submod_defaults()["root"]), "Wrapper")

        chains = ChainDriver().call_graph_request().conditional_wiring()
        self.assertEqual(len(chains), 1)
        branches = chains[0].branches()
        self.assertEqual(len(branches), 3)
        self.assertEqual(branches[0].input_key(), "mode")
        self.assertEqual(branches[0].value(), "plain")
        self.assertFalse(branches[2].has_condition())
        self.assertEqual(branches[2].requests()[0].value(), 100)

    def test_module_api(self):
        self.assertTrue(self.mm.at("X driver").is_driver())
        self.assertFalse(self.mm.at("Wrapper").is_driver())
        self.assertEqual(self.mm.at("X driver").inactive_settings(), [])
        self.assertTrue(self.mm.at("X driver").call_graph_request().empty())
        self.assertRaises(
            RuntimeError, self.mm.at("Wrapper").call_graph_request
        )

    def test_must_run_through_module_manager(self):
        driver = self.mm.at("X driver")
        self.assertRaises(RuntimeError, driver.run_as, self.pt)
        self.assertEqual(self.run_as("X driver"), 2)
        self.assertEqual(driver.run_as(self.pt), 2)

    def test_forwarding(self):
        self.mm.change_input("X driver", "x", 5)
        self.assertEqual(self.run_as("X driver"), 6)
        # The ModuleManager's modules are not changed
        self.assertEqual(self.mm.at("Leaf").inputs()["x"].value(), 1)
        self.assertEqual(self.run_as("Wrapper"), 2)
        self.assertTrue(self.mm.at("Leaf").locked())

    def test_user_requests(self):
        self.mm.change_input("X driver", "x", 5)
        request = self.mm.at("X driver").call_graph_request()
        request.set_input("Leaf", "x", 20)
        request.set_input("Adder", "offset", 3).required()
        self.assertTrue(request.requests()[1].is_required())
        self.assertEqual(self.run_as("X driver"), 24)
        self.assertRaises(
            RuntimeError, self.mm.at("X driver").call_graph_request
        )

    def test_inactive_settings(self):
        self.mm.change_submod("X driver", "root", "Other leaf")
        self.assertEqual(self.run_as("X driver"), 1)
        self.assertEqual(
            self.mm.at("X driver").inactive_settings(),
            [
                '"X driver": forwarding input "x" to "Leaf"\'s input "x" '
                'has no effect because "Leaf" is not in the call graph'
            ],
        )

    def test_chains(self):
        self.assertEqual(self.run_as("Chain driver"), 2)

        self.mm.copy_module("Chain driver", "Double")
        self.mm.change_input("Double", "mode", "double")
        self.mm.change_input("Double", "offset", 10)
        self.assertEqual(self.run_as("Double"), 12)

        self.mm.copy_module("Chain driver", "Other")
        self.mm.change_input("Other", "mode", "other")
        self.assertEqual(self.run_as("Other"), 102)

        self.mm.copy_module("Chain driver", "Plain")
        self.mm.change_input("Plain", "offset", 10)
        self.assertEqual(self.run_as("Plain"), 2)
        self.assertEqual(
            self.mm.at("Plain").inactive_settings(),
            [
                '"Plain": "offset" has no effect because no branch that '
                "forwards it was taken"
            ],
        )
