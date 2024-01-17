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
        ppe.load_modules(mm)
        self.assertEqual(mm.size(),4)
        
        module_names = ["Coulomb's Law", "Classical Force", 
                        "Coulomb's Law with screening", 
                        "Single-precision Coulomb's law"]
        for name in module_names:
            self.assertTrue(mm.count(name))

        force0_mod = mm.at("Classical Force")
        force1_mod = mm.at("Coulomb's Law")
        force2_mod = mm.at("Single-precision Coulomb's law")
        force3_mod = mm.at("Coulomb's Law with screening")

        p0 = [0.0, 0.0, 0.0]
        p1 = [1.0, 0.0, 0.0]
        p2 = [2.0, 0.0, 0.0]
        r0 = [5.5, 0.0, 0.0]
        r1 = [1.5, 0.0, 0.0]
        pc0 = ppe.PointCharge(1.0, p0)
        pc1 = ppe.PointCharge(1.0, p1)
        pc2 = ppe.PointCharge(2.0, p2)
        pcs = [pc1, pc2]
        m1 = 2.0

        force_pt = ppe.Force()
        efield_pt = ppe.ElectricField()

        result0 = force0_mod.run_as(force_pt, pc0, m1, p2, pcs)
        self.assertTrue(result0 == r0)

        result1 = force1_mod.run_as(efield_pt, p0, pcs)
        self.assertTrue(result1 == r1)

        result2 = force2_mod.run_as(efield_pt, p0, pcs)
        self.assertTrue(result2 == r1)

        result3 = force3_mod.run_as(efield_pt, p0, pcs)
        self.assertTrue(result3 == r1)

        force3_mod_copy = force3_mod.unlocked_copy()
        force3_mod_copy.change_input("threshold", 1.0)
        screened_result3 = force3_mod_copy.run_as(efield_pt, p0, pcs)
        self.assertTrue(screened_result3 == p0)
