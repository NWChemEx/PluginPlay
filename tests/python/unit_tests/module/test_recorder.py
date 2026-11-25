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


class Wrapped(pp.ModuleBase):
    """Doubles its input, and keeps the inputs it is called with."""

    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.satisfies_property_type(test_pp.OneInOneOut())
        self.inputs_seen = []

    def run_(self, inputs, submods):
        pt = test_pp.OneInOneOut()
        (i,) = pt.unwrap_inputs(inputs)
        self.inputs_seen.append(i)
        rv = self.results()
        return pt.wrap_results(rv, 2 * i)


class TestRecorder(unittest.TestCase):
    def test_is_a_module(self):
        self.assertIsInstance(self.recorder, pp.ModuleBase)

    def test_no_records_before_it_is_run(self):
        self.assertEqual(self.recorder.records(), [])

    def test_forwards_inputs_and_returns_results(self):
        self.assertEqual(self.mm.run_as(self.pt, "Recorder", 3), 6)
        self.assertEqual(self.wrapped.inputs_seen, [3])

    def test_records_each_run_in_order(self):
        self.mm.run_as(self.pt, "Recorder", 3)
        self.mm.run_as(self.pt, "Recorder", 4)

        records = self.recorder.records()
        self.assertEqual(len(records), 2)
        self.assertEqual(self.pt.unwrap_results(records[0]), (6,))
        self.assertEqual(self.pt.unwrap_results(records[1]), (8,))

    def test_clear(self):
        self.mm.run_as(self.pt, "Recorder", 3)
        self.recorder.clear()
        self.assertEqual(self.recorder.records(), [])

    def setUp(self):
        self.pt = test_pp.OneInOneOut()
        self.mm = pp.ModuleManager()
        self.wrapped = Wrapped()
        self.recorder = pp.Recorder(self.pt)
        self.mm.add_module("Wrapped", self.wrapped)
        self.mm.add_module("Recorder", self.recorder)
        self.mm.change_submod("Recorder", "Wrapped", "Wrapped")
