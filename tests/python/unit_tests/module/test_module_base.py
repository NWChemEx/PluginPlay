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
import parallelzone as pz
import py_test_pluginplay as test_pp
import unittest


class APythonModule(pp.ModuleBase):

    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.satisfies_property_type(test_pp.OneInOneOut())
        self.description("This is a Python Module!!!")
        self.citation("A. Author. The most awesome-est paper ever.")
        self.add_input("An extra input")
        self.add_result("An extra result")
        self.add_submodule(test_pp.OneInOneOut(), "A submodule")

    def run_(self, inputs, submods):
        pt = test_pp.OneInOneOut()
        i0, = pt.unwrap_inputs(inputs)
        i1 = inputs["An extra input"].value()

        assert (self.get_runtime() == pz.runtime.RuntimeView())
        r0 = submods["A submodule"].run_as(pt, i0 + i1)

        rv = self.results()
        rv = pt.wrap_results(rv, r0)
        rv['An extra result'].change(42)
        return rv


class TestModuleBase(unittest.TestCase):

    def test_results(self):
        self.assertEqual(self.defaulted.results(), {})
        self.assertNotEqual(self.py_mod.results(), {})

    def test_run_as(self):
        self.mm.change_input(self.mod_key, 'An extra input', 2)
        r = self.mm.run_as(test_pp.OneInOneOut(), self.mod_key, 1)
        self.assertEqual(r, 3)

    def setUp(self):
        self.defaulted = pp.ModuleBase()
        self.mm = test_pp.get_mm()
        self.mod_key = 'Python module'
        self.py_mod = APythonModule()
        self.mm.add_module(self.mod_key, self.py_mod)
        submod_key = 'C++ module using every feature'
        self.mm.change_submod(self.mod_key, "A submodule", submod_key)
