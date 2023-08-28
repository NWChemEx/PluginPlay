#
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
#
import parallelzone as pz
import unittest
import pluginplay as pp
import pluginplay_examples as ppe

class TestTutorialModules(unittest.TestCase):
    def test_modules(self):
        rv = pz.runtime.RuntimeView()
        mm = pp.ModuleManager()
        self.assertEqual(mm.size(),0)
        ppe.load_modules(mm)
        self.assertEqual(mm.size(),4)
        module_names = ["Coulomb's Law", "Force", "Coulomb's Law with screening", "Single-precision Coulomb's law"]
        for name in module_names:
            mod = mm.at(name)
            self.assertTrue(mod.has_description())
