# Copyright 2022 NWChemEx-Project
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
import pluginplay_examples as ppe
import coulombslaw_force as clf
import unittest

class TestNewPythonModules(unittest.TestCase):
    def test_modules(self):
        p1 = ppe.PointCharge(1.0, [1.0, 0.0, 0.0])
        p2 = ppe.PointCharge(2.0, [2.0, 0.0, 0.0])
        pvc = [p1, p2]
        r = [0.0, 0.0, 0.0]
        q = ppe.PointCharge(1.0, [0.0, 0.0, 0.0])
        m = 2.0
        a = [2.0, 0.0, 0.0]

        mm = pp.ModuleManager()
        ppe.load_modules(mm)

        # There's a bug here that's messing up the property types somehow.
        # The property seems to get "stuck" on the first add_module call
        mm.add_module("My Coulomb's Law", clf.CoulombsLaw())
        # mm.add_module("My Force", clf.ClassicalForce())
        # mm.change_submod("My Force", "electric field", "Coulomb's Law")

        field = mm.at("My Coulomb's Law").run_as(ppe.ElectricField(), r, pvc)
        self.assertTrue(field == [1.5, 0.0, 0.0])
        # cforce = mm.at("My Force").run_as(ppe.Force(), q, m, a, pvc)
        # self.assertTrue(cforce == [5.5, 0.0, 0.0])
