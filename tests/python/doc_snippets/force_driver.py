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

from math import inf

import pluginplay as pp
import pluginplay_examples as ppe


class ForceDriver(pp.DriverBase):
    def __init__(self):
        pp.DriverBase.__init__(self)
        self.description("Classical force with an optionally screened field")
        self.satisfies_property_type(ppe.Force())
        self.add_submodule(ppe.Force(), "force").set_description(
            "The module that computes the force"
        )

        self.add_input("screen").set_description(
            "Should the electric field be screened?"
        ).set_default(False)
        self.add_input("threshold").set_description(
            "maximum ||r-r_i|| for contributing to electric field"
        ).set_default(inf)

        # Use the screened electric field, and its threshold, if "screen"
        # is True
        self.when("screen", True).set_submod(
            "Classical Force", "electric field", "Coulomb's Law with screening"
        ).forward_input(
            "Coulomb's Law with screening", "threshold", "threshold"
        ).otherwise().set_submod(
            "Classical Force", "electric field", "Coulomb's Law"
        )

    def run_(self, inputs, submods):
        pt = ppe.Force()
        [q, m, a, charges] = pt.unwrap_inputs(inputs)
        F = submods["force"].run_as(pt, q, m, a, charges)
        return pt.wrap_results(self.results(), F)


class FieldChoiceDriver(pp.DriverBase):
    def __init__(self):
        pp.DriverBase.__init__(self)
        self.description("Classical force with a user-chosen electric field")
        self.satisfies_property_type(ppe.Force())
        self.add_submodule(ppe.Force(), "force").set_default("Classical Force")

        self.add_submodule(ppe.ElectricField(), "electric field").set_default(
            "Coulomb's Law"
        ).set_target("Classical Force", "electric field")

    def run_(self, inputs, submods):
        pt = ppe.Force()
        [q, m, a, charges] = pt.unwrap_inputs(inputs)
        F = submods["force"].run_as(pt, q, m, a, charges)
        return pt.wrap_results(self.results(), F)
