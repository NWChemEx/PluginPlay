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

import py_test_pluginplay as test_pp

import pluginplay as pp


class TestModule(unittest.TestCase):
    def test_unlocked_copy(self):
        other_defaulted = self.defaulted.unlocked_copy()
        self.assertEqual(self.defaulted, other_defaulted)

        self.ready_mod.lock()
        other_ready = self.ready_mod.unlocked_copy()
        self.assertFalse(other_ready.locked())
        self.assertNotEqual(other_ready, self.ready_mod)

    def test_has_module(self):
        self.assertFalse(self.defaulted.has_module())
        self.assertTrue(self.has_desc.has_module())
        self.assertTrue(self.has_cite.has_module())
        self.assertTrue(self.not_ready.has_module())
        self.assertTrue(self.need_submod.has_module())
        self.assertTrue(self.ready_mod.has_module())

    def test_has_description(self):
        # See issue #301
        # self.assertFalse(self.defaulted.has_description())

        self.assertTrue(self.has_desc.has_description())
        self.assertFalse(self.has_cite.has_description())
        self.assertFalse(self.not_ready.has_description())
        self.assertFalse(self.need_submod.has_description())
        self.assertFalse(self.ready_mod.has_description())

    def test_has_name(self):
        self.assertFalse(self.defaulted.has_name())
        self.assertTrue(self.has_desc.has_name())

    def test_locked(self):
        self.assertFalse(self.defaulted.locked())
        self.assertFalse(self.has_desc.locked())
        self.assertFalse(self.has_cite.locked())
        self.assertFalse(self.not_ready.locked())
        self.assertFalse(self.need_submod.locked())
        self.assertFalse(self.ready_mod.locked())

    def test_list_not_ready(self):
        # Throws if there's no implementation b/c default constructed
        self.assertRaises(Exception, self.defaulted.list_not_ready)
        self.assertEqual(self.has_desc.list_not_ready(), {})
        self.assertEqual(self.has_cite.list_not_ready(), {})

        # Has two inputs that aren't set
        corr = {"Inputs": {"Option 2", "Option 1"}}
        self.assertEqual(self.not_ready.list_not_ready(), corr)

        # If one input comes from say a property type
        corr = {"Inputs": {"Option 2"}}
        inps = test_pp.OneIn().inputs()
        self.assertEqual(self.not_ready.list_not_ready(inps), corr)

        # Not ready b/c of an unset submodule
        corr = {
            "Submodules": {
                "Submodule 1",
            }
        }
        self.assertEqual(self.need_submod.list_not_ready(), corr)

        # Is actually ready to run
        self.assertEqual(self.ready_mod.list_not_ready(), {})

    def test_ready(self):
        self.assertFalse(self.not_ready.ready())
        self.assertTrue(self.ready_mod.ready())

        pt = test_pp.TwoIn()
        inps = pt.inputs()
        self.assertTrue(self.not_ready.ready(pt))
        self.assertTrue(self.not_ready.ready(inps))

    def test_reset_cache(self):
        # The main cache lives in the PIMPL, not the implementation (so this
        # should work)
        self.defaulted.reset_cache()

        # For now just testing if we can call the function
        self.has_desc.reset_cache()

    def test_reset_internal_cache(self):
        # Throws if doesn't have cache b/c default constructed
        self.assertRaises(Exception, self.defaulted.reset_internal_cache)

        # For now just testing if we can call the function
        self.has_desc.reset_internal_cache()

    def test_is_memoizable(self):
        # Throws if there's no implementation b/c default constructed
        self.assertRaises(Exception, self.defaulted.is_memoizable)

        self.assertTrue(self.has_desc.is_memoizable())
        self.assertTrue(self.has_cite.is_memoizable())
        self.assertTrue(self.not_ready.is_memoizable())

        # Throws if submodule is not set yet
        self.assertRaises(Exception, self.need_submod.is_memoizable)

        self.assertTrue(self.ready_mod.is_memoizable())

    def test_turn_off_memoization(self):
        self.assertTrue(self.has_desc.is_memoizable())
        self.has_desc.turn_off_memoization()
        self.assertFalse(self.has_desc.is_memoizable())

    def test_turn_on_memoization(self):
        self.has_desc.turn_off_memoization()
        self.assertFalse(self.has_desc.is_memoizable())
        self.has_desc.turn_on_memoization()
        self.assertTrue(self.has_desc.is_memoizable())

    def test_lock(self):
        # See issue #301
        # self.assertRaises(Exception, self.defaulted.lock)

        # Can't lock a not ready module
        self.assertRaises(Exception, self.need_submod.lock)

        # Can lock a ready module
        self.assertFalse(self.ready_mod.locked())
        self.ready_mod.lock()
        self.assertTrue(self.ready_mod.locked())

    def test_results(self):
        # Throws if there's no implementation
        self.assertRaises(Exception, self.defaulted.results)

        # Modules which don't return results
        self.assertEqual(self.has_desc.results(), {})
        self.assertEqual(self.has_cite.results(), {})
        self.assertEqual(self.not_ready.results(), {})
        self.assertEqual(self.need_submod.results(), {})

        # Module that actually returns results
        corr = test_pp.OptionalInput().results()
        self.assertEqual(self.ready_mod.results(), corr)

    def test_inputs(self):
        # Throws if there's no implementation
        self.assertRaises(Exception, self.defaulted.inputs)

        # Modules with no inputs
        self.assertEqual(self.has_desc.inputs(), {})
        self.assertEqual(self.has_cite.inputs(), {})
        self.assertEqual(self.need_submod.inputs(), {})

        # Module with inputs
        corr = test_pp.OptionalInput().inputs()
        self.assertEqual(self.ready_mod.inputs(), corr)

    def test_submods(self):
        # Throws if there's no implementation
        self.assertRaises(Exception, self.defaulted.submods)

        self.assertEqual(self.has_desc.submods(), {})
        self.assertEqual(self.has_cite.submods(), {})
        self.assertEqual(self.not_ready.submods(), {})
        self.assertIn("Submodule 1", self.need_submod.submods())
        self.assertEqual(self.ready_mod.submods(), {})

    def test_property_types(self):
        # Throws if there's no implementation
        self.assertRaises(Exception, self.defaulted.property_types)

        null_pt = test_pp.NullPT().type()
        self.assertEqual(
            self.has_desc.property_types(),
            {
                null_pt,
            },
        )
        self.assertEqual(
            self.has_cite.property_types(),
            {
                null_pt,
            },
        )
        self.assertEqual(
            self.need_submod.property_types(),
            {
                null_pt,
            },
        )

        one_in_pt = test_pp.OneIn().type()
        self.assertEqual(
            self.not_ready.property_types(),
            {
                one_in_pt,
            },
        )

        opt_pt = test_pp.OptionalInput().type()
        self.assertEqual(
            self.ready_mod.property_types(),
            {
                opt_pt,
            },
        )

    def test_description(self):
        # Throws if there's no implementation
        self.assertRaises(Exception, self.defaulted.description)

        self.assertEqual(self.has_desc.description(), "A description")

        # Throws if there's no description
        self.assertRaises(Exception, self.has_cite.description)
        self.assertRaises(Exception, self.not_ready.description)
        self.assertRaises(Exception, self.need_submod.description)
        self.assertRaises(Exception, self.ready_mod.description)

    def test_citations(self):
        # Throws if there's no implementation
        self.assertRaises(Exception, self.defaulted.citations)

        self.assertEqual(self.has_desc.citations(), [])
        self.assertEqual(self.has_cite.citations(), ["A citation"])
        self.assertEqual(self.not_ready.citations(), [])
        self.assertEqual(self.need_submod.citations(), [])
        self.assertEqual(self.ready_mod.citations(), [])

    def test_get_name(self):
        self.assertRaises(RuntimeError, self.defaulted.get_name)
        self.defaulted.set_name("test")
        self.assertEqual(self.defaulted.get_name(), "test")

    def test_set_name(self):
        self.has_desc.set_name("test")
        self.assertEqual(self.has_desc.get_name(), "test")

    def test_change_input(self):
        # Throws if there's no implementation
        self.assertRaises(Exception, self.defaulted.change_input, "hello")

        # Throws if there's no input w/ provided name
        not_ready = self.not_ready
        self.assertRaises(Exception, not_ready.change_input, "nope", 3)

        # Throws if input is not convertible
        self.assertRaises(Exception, not_ready.change_input, "Option 2", [])

        # Can change an input
        self.not_ready.change_input("Option 2", 3)
        self.assertEqual(not_ready.inputs()["Option 2"].value(), 3)

    def test_change_submod(self):
        # Throws if there's not implementation
        self.assertRaises(Exception, self.defaulted.change_submod, "hello")

        # Throws if there's no submodule with the key
        submod = self.has_desc
        has_submod = self.need_submod
        key = "Submodule 1"
        self.assertRaises(Exception, has_submod.change_submod, "nope", submod)

        # Throws if submod is wrong PT
        wrong_pt = self.not_ready
        self.assertRaises(Exception, has_submod.change_submod, key, wrong_pt)

        # Can actually change the submod
        has_submod.change_submod(key, submod)
        self.assertEqual(has_submod.submods()[key].value(), submod)

    def test_run_as(self):
        # Can't run a module with no implementation
        self.assertRaises(Exception, self.defaulted.run_as, test_pp.NullPT())

        # Can't run a module as a PT it doesn't satisfy
        self.assertRaises(Exception, self.real_deal.run_as, test_pp.NullPT())

        # Throws if input is wrong type
        pt = test_pp.OneInOneOut()
        self.assertRaises(Exception, self.real_deal.run_as, pt, [42])

        # Works with correct PT and a valid input
        rv = self.real_deal.run_as(pt, 42)
        self.assertEqual(rv, 42)

    def test_run(self):
        # Throws if no implementation
        self.assertRaises(Exception, self.defaulted.run, {})

        # Throws if module is not ready
        self.assertRaises(Exception, self.not_ready.run, {})

        # Can call ready module
        rv = self.ready_mod.run({})
        self.assertEqual(rv["Result 1"].value(), 1)

    def test_profile_info(self):
        # No implementation means no profile info
        self.assertEqual(self.defaulted.profile_info(), "")

        # Module which hasn't run, has no profile info
        self.assertEqual(self.ready_mod.profile_info(), "")

        # A run module has profile info
        rv = self.ready_mod.run_as(test_pp.OptionalInput())
        self.assertEqual(rv, 1)
        self.assertNotEqual(self.ready_mod.profile_info(), "")

    def test_submod_uuids(self):
        # Throws if no module is set
        self.assertRaises(Exception, self.defaulted.submod_uuids)

        # Module has no submodules
        self.assertEqual(self.not_ready.submod_uuids(), {})

        # Throws if module needs submodules, but doesn't have one yet
        self.assertRaises(Exception, self.need_submod.submod_uuids)

        # Module has a set submodule
        self.need_submod.change_submod("Submodule 1", self.has_desc)
        corr = {"Submodule 1": self.has_desc.uuid()}
        self.assertEqual(self.need_submod.submod_uuids(), corr)

    def test_uuid(self):
        # Throws if no module is set
        self.assertRaises(Exception, self.defaulted.uuid)

        # Not really sure how else to test the UUIDs since they'll be different
        # each invocation...
        self.assertNotEqual(self.has_desc.uuid(), None)
        self.assertNotEqual(self.has_cite.uuid(), None)
        self.assertNotEqual(self.not_ready.uuid(), None)
        self.assertNotEqual(self.need_submod.uuid(), None)
        self.assertNotEqual(self.ready_mod.uuid(), None)
        self.assertNotEqual(self.real_deal.uuid(), None)

    def test_comparisons(self):
        # Default vs default
        other_default = pp.Module()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Default vs non-default
        self.assertNotEqual(self.defaulted, self.has_desc)
        self.assertFalse(self.defaulted == self.has_desc)

    def setUp(self):
        mm = test_pp.get_mm()
        self.defaulted = pp.Module()
        self.has_desc = mm.at("C++ with description")
        self.has_cite = mm.at("C++ with a citation")
        self.not_ready = mm.at("C++ module which also isn't ready")
        self.need_submod = mm.at("C++ module needing a submodule")
        self.ready_mod = mm.at("C++ module which is ready to run")
        self.real_deal = mm.at("C++ module using every feature")


class TestPrintNotReady(unittest.TestCase):
    def test_print_not_ready(self):
        # Raises an error if module has no implementation
        self.assertRaises(Exception, pp.print_not_ready, pp.Module())
