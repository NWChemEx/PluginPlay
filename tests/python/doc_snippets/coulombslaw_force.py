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
from math import sqrt


class CoulombsLaw(pp.ModuleBase):

    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.description("Electric Field From Coulomb's Law")
        self.satisfies_property_type(ppe.ElectricField())

    def run_(self, inputs, submods):
        pt = ppe.ElectricField()
        [r, charges] = pt.unwrap_inputs(inputs)
        E = [0.0, 0.0, 0.0]
        for charge in charges:
            q = charge.m_charge
            ri = charge.m_r
            ri2 = ri[0]**2 + ri[1]**2 + ri[2]**2
            mag_ri = sqrt(ri2)
            rij = [i for i in r]
            for i in range(3):
                rij[i] -= charge.m_r[i]
            rij2 = rij[0]**2 + rij[1]**2 + rij[2]**2
            for i in range(3):
                E[i] += (q * ri[i] / (mag_ri * rij2))
        rv = self.results()
        return pt.wrap_results(rv, E)


class ClassicalForce(pp.ModuleBase):

    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.description("Classical Force Field")
        self.satisfies_property_type(ppe.Force())
        self.add_submodule(
            ppe.ElectricField(), "electric field").set_description(
                "Used to compute the electric field of the point charges")

    def run_(self, inputs, submods):
        pt = ppe.Force()
        [q, m, a, charges] = pt.unwrap_inputs(inputs)
        F = [0.0, 0.0, 0.0]
        E = submods["electric field"].run_as(ppe.ElectricField(), q.m_r,
                                             charges)
        for i in range(3):
            F[i] = m * a[i] + q.m_charge * E[i]
        rv = self.results()
        return pt.wrap_results(rv, F)
