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

import coulombslaw_force as clf
import force_driver as fd
import pluginplay as pp
import pluginplay_examples as ppe


def make_mm():
    """The modules the tutorial's drivers configure, written in Python"""
    mm = pp.ModuleManager()
    mm.add_module("Coulomb's Law", clf.CoulombsLaw())
    mm.add_module("Coulomb's Law with screening", clf.ScreenedCoulombsLaw())
    mm.add_module("Classical Force", clf.ClassicalForce())
    mm.change_submod("Classical Force", "electric field", "Coulomb's Law")
    return mm


class TestDrivers(unittest.TestCase):
    def setUp(self):
        # A particle with charge 1 at the origin, and two point charges
        self.q = ppe.PointCharge(1.0, [0.0, 0.0, 0.0])
        self.m = 2.0
        self.a = [2.0, 0.0, 0.0]
        self.charges = [
            ppe.PointCharge(1.0, [1.0, 0.0, 0.0]),
            ppe.PointCharge(2.0, [2.0, 0.0, 0.0]),
        ]
        self.pt = ppe.Force()

    def test_using_a_driver(self):
        q, m, a, charges, pt = self.q, self.m, self.a, self.charges, self.pt

        mm = pp.ModuleManager()
        mm.add_module("Coulomb's Law", clf.CoulombsLaw())
        mm.add_module(
            "Coulomb's Law with screening", clf.ScreenedCoulombsLaw()
        )
        mm.add_module("Classical Force", clf.ClassicalForce())
        mm.change_submod("Classical Force", "electric field", "Coulomb's Law")
        mm.add_module("Force driver", fd.ForceDriver())
        mm.change_submod("Force driver", "force", "Classical Force")

        # Run with the default settings
        F = mm.run_as(pt, "Force driver", q, m, a, charges)

        self.assertEqual(F, [5.5, 0.0, 0.0])
        self.assertEqual(mm.at("Force driver").inactive_settings(), [])

        # Use other settings by copying the driver
        mm.copy_module("Force driver", "Screened force driver")
        mm.change_input("Screened force driver", "screen", True)
        mm.change_input("Screened force driver", "threshold", 1.5)
        F_screened = mm.run_as(pt, "Screened force driver", q, m, a, charges)

        self.assertEqual(F_screened, [5.0, 0.0, 0.0])
        screened = mm.at("Screened force driver")
        self.assertEqual(screened.inactive_settings(), [])

        # The ModuleManager's modules are not changed by the driver
        efield = mm.at("Classical Force").submods()["electric field"].value()
        self.assertEqual(efield.get_name(), "Coulomb's Law")
        self.assertEqual(mm.run_as(pt, "Force driver", q, m, a, charges), F)

        # Change "threshold" without turning on "screen"
        mm.copy_module("Force driver", "Unscreened force driver")
        mm.change_input("Unscreened force driver", "threshold", 1.5)
        mm.run_as(pt, "Unscreened force driver", q, m, a, charges)

        inactive = mm.at("Unscreened force driver").inactive_settings()
        # "threshold" is inactive: the branch that forwards it was not taken

        self.assertEqual(
            inactive,
            [
                '"Unscreened force driver": "threshold" has no effect because '
                "no branch that forwards it was taken"
            ],
        )

        # Override one of the driver's choices
        mm.copy_module("Force driver", "My force driver")
        request = mm.at("My force driver").call_graph_request()
        request.set_submod(
            "Classical Force", "electric field", "Coulomb's Law with screening"
        )
        request.set_input("Coulomb's Law with screening", "threshold", 1.5)
        F_mine = mm.run_as(pt, "My force driver", q, m, a, charges)

        self.assertEqual(F_mine, [5.0, 0.0, 0.0])

    def test_wiring_a_drivers_submodule(self):
        q, m, a, charges, pt = self.q, self.m, self.a, self.charges, self.pt

        mm = make_mm()
        mm.change_input("Coulomb's Law with screening", "threshold", 1.5)
        mm.add_module("Field choice driver", fd.FieldChoiceDriver())

        # The defaults of the driver's submodules are used
        F = mm.run_as(pt, "Field choice driver", q, m, a, charges)
        self.assertEqual(F, [5.5, 0.0, 0.0])

        # The submodule the user binds is passed on to "Classical Force"
        mm.copy_module("Field choice driver", "Screened field choice driver")
        mm.change_submod(
            "Screened field choice driver",
            "electric field",
            "Coulomb's Law with screening",
        )
        F = mm.run_as(pt, "Screened field choice driver", q, m, a, charges)
        self.assertEqual(F, [5.0, 0.0, 0.0])
