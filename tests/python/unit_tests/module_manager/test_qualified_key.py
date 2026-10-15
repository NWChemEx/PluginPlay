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


class TestQualifiedKey(unittest.TestCase):
    def test_key_only(self):
        key = pp.QualifiedKey("Overlap")
        self.assertFalse(key.is_qualified())
        self.assertEqual(key.plugin(), "")
        self.assertEqual(key.key(), "Overlap")
        self.assertEqual(str(key), "Overlap")

    def test_qualified(self):
        key = pp.QualifiedKey("integrals", "Overlap")
        self.assertTrue(key.is_qualified())
        self.assertEqual(key.plugin(), "integrals")
        self.assertEqual(key.key(), "Overlap")
        self.assertEqual(str(key), "integrals::Overlap")

    def test_from_tuple(self):
        key = pp.QualifiedKey(("integrals", "Overlap"))
        self.assertEqual(key, pp.QualifiedKey("integrals", "Overlap"))
        self.assertRaises(ValueError, pp.QualifiedKey, ("a", "b", "c"))

    def test_comparisons(self):
        key = pp.QualifiedKey("integrals", "Overlap")
        self.assertEqual(key, pp.QualifiedKey("integrals", "OVERLAP"))
        self.assertEqual(key, pp.QualifiedKey("Integrals", "Overlap"))
        self.assertNotEqual(key, pp.QualifiedKey("nux", "Overlap"))
        self.assertNotEqual(key, pp.QualifiedKey("Overlap"))
        self.assertNotEqual(pp.QualifiedKey("", "a"), pp.QualifiedKey("a"))
