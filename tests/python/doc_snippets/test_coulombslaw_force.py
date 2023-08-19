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
from pluginplay_examples import ElectricField, Force, Point, PointCharge
from math import sqrt
from coulombslaw_force import CoulombsLaw, ClassicalForce

class TestCoulombsLaw(unittest.TestCase):
    def test_coulomb(self):
        rv = pz.runtime.RuntimeView()
        mm = pp.ModuleManager()

        mm.add_module("Force", ClassicalForce())
        mm.add_module("Coulomb's Law", CoulombsLaw())
        mm.change_submod("Force", "electric field", "Coulomb's Law")

        r = Point([0.0, 0.0, 0.0])

        pvc = [PointCharge(1.0, [2.0, 3.0, 3.0]), PointCharge(0.5, [-1.0, 4.0, 0.0])]
        field = mm.at("Coulomb's Law").run_as(ElectricField, r, pvc)

        q = PointCharge(0.1, [1.0, 1.0, 1.0])
        m = 3.0
        a = Point([-1.0, -1.0, -1.0])
        cforce = mm.at("Force").run_as(Force, q, m, a, pvc)
        print(cforce)
