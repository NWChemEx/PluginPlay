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
import py_test_pluginplay as test_pp
import py_test_pluginplay.test_submodule_request as test_sr
import unittest


class TestSubmoduleRequest(unittest.TestCase):

    def test_has_type(self):
        self.assertFalse(self.defaulted.has_type())
        self.assertTrue(self.expects_null.has_type())
        self.assertTrue(self.expects_one_in.has_type())
        self.assertTrue(self.not_ready_mod.has_type())
        self.assertTrue(self.ready_mod.has_type())

    def test_has_module(self):
        self.assertFalse(self.defaulted.has_module())
        self.assertFalse(self.expects_null.has_module())
        self.assertFalse(self.expects_one_in.has_module())
        self.assertTrue(self.not_ready_mod.has_module())
        self.assertTrue(self.ready_mod.has_module())

    def test_has_description(self):
        self.assertFalse(self.defaulted.has_description())
        self.assertFalse(self.expects_null.has_description())
        self.assertFalse(self.expects_one_in.has_description())
        self.assertFalse(self.not_ready_mod.has_description())
        self.assertFalse(self.ready_mod.has_description())

    def test_has_name(self):
        self.assertFalse(self.defaulted.has_name())
        self.assertTrue(self.ready_mod.has_name())

    def test_ready(self):
        self.assertFalse(self.defaulted.ready())
        self.assertFalse(self.expects_null.ready())
        self.assertFalse(self.expects_one_in.ready())
        self.assertFalse(self.not_ready_mod.ready())
        self.assertTrue(self.ready_mod.ready())

    def test_set_type(self):
        self.assertFalse(self.defaulted.has_type())
        self.defaulted.set_type(test_pp.NullPT())
        self.assertTrue(self.defaulted.has_type())

    def test_satisfies_property_type(self):
        pt1 = test_pp.NullPT()
        pt2 = test_pp.OneIn()
        self.assertRaises(RuntimeError, self.defaulted.satisfies_property_type,
                          pt1)
        self.defaulted.set_type(pt1)
        self.assertTrue(self.defaulted.satisfies_property_type(pt1))
        self.assertFalse(self.defaulted.satisfies_property_type(pt2))

    def test_set_description(self):
        self.assertFalse(self.defaulted.has_description())
        self.defaulted.set_description("Hello World!")
        self.assertTrue(self.defaulted.has_description())
        self.assertEqual(self.defaulted.description(), "Hello World!")

    def test_submod_uuids(self):
        # Throws if no module is set
        self.assertRaises(Exception, self.defaulted.submod_uuids)

    def test_uuid(self):
        # Throws if no module is set
        self.assertRaises(Exception, self.defaulted.uuid)

    def test_value(self):
        # Throws if no module is set
        self.assertRaises(Exception, self.defaulted.value)
        self.assertNotEqual(self.ready_mod.value(), None)

    def test_description(self):
        # Throws if no description is set
        self.assertRaises(Exception, self.defaulted.description)
        self.defaulted.set_description("Hello World!")
        self.assertEqual(self.defaulted.description(), "Hello World!")

    def test_get_name(self):
        self.assertRaises(RuntimeError, self.defaulted.get_name)
        self.assertTrue(self.ready_mod.get_name(), "a_mod")

    def test_lock(self):
        # Throws if no module is set
        self.assertRaises(Exception, self.defaulted.lock)
        self.ready_mod.lock()
        self.assertTrue(self.ready_mod.value().locked())

    def test_comparisons(self):
        # Default vs default
        other_default = pp.SubmoduleRequest()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)

        # Default vs non-default
        self.assertNotEqual(self.defaulted, self.expects_null)
        self.assertFalse(self.defaulted == self.expects_null)

        # Non-default vs non-default
        other_null = pp.SubmoduleRequest()
        other_null.set_type(test_pp.NullPT())
        self.assertEqual(self.expects_null, other_null)
        self.assertFalse(self.expects_null != other_null)

        # Different descriptions
        other_null.set_description("Hello World!")
        self.assertNotEqual(self.expects_null, other_null)
        self.assertFalse(self.expects_null == other_null)

        # different PTs
        self.assertNotEqual(self.expects_null, self.expects_one_in)
        self.assertFalse(self.expects_null == self.expects_one_in)

    def setUp(self):
        self.defaulted = pp.SubmoduleRequest()

        self.expects_null = pp.SubmoduleRequest()
        self.expects_null.set_type(test_pp.NullPT())

        self.expects_one_in = pp.SubmoduleRequest()
        self.expects_one_in.set_type(test_pp.OneIn())

        self.not_ready_mod = test_sr.get_sr_w_not_ready_mod()
        self.ready_mod = test_sr.get_sr_w_ready_mod()
