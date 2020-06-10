from sde import *
from tests import *
import unittest

mm = sde.ModuleManager()
cppyy.gbl.load_modules(mm)

def get_rectangle():
    mod = mm.at("Rectangle")
    return sde.Module(mod)

def get_prism():
    mod = mm.at("Prism")
    return sde.Module(mod)

class test_module(unittest.TestCase):
    def setUp(self):
        self.r1 = get_rectangle()
        self.r2 = get_rectangle()
        self.p1 = get_prism()
        self.p2 = get_prism()

    def test_default_ctor(self):
        m = sde.Module()
        self.assertFalse(m.locked())

    def test_copy_ctor(self):
        m = sde.Module(self.r1)
        self.assertEqual(self.r1, m)

    def test_equality(self):
        m1 = sde.Module()
        m2 = sde.Module()
        self.assertEqual(m1, m2)

    def test_lockedness(self):
        self.assertEqual(self.r1, self.r2)
        self.r2.lock()
        self.assertNotEqual(self.r1, self.r2)

    def test_change_input(self):
        self.r1.change_input("Dimension 1", 1.23)
        self.assertNotEqual(self.r1, self.r2)

    def test_run_as(self):
        area = self.r1.run_as["Area"](1.23, 4.56)
        self.assertEqual(area[0], 1.23*4.56)

    def test_run(self):
        inputs = self.r1.inputs()
        inputs.at("Dimension 1").change(1.23)
        inputs.at("Dimension 2").change(4.56)
        rv = self.r1.run(inputs)
        self.assertEqual(rv.at("area").value["double"](), 1.23*4.56)

    def test_ready(self):
        self.assertFalse(self.r1.ready())
        self.assertTrue(self.r1.ready["Area"]())

    def test_lock(self):
        self.assertFalse(self.r1.locked())
        self.r1.lock()
        self.assertTrue(self.r1.locked())

    def test_change_submod(self):
        self.p1.change_submod("area", get_rectangle())
        self.assertEqual(self.p1.submods().at("area").value(), get_rectangle())

if __name__ == "__main__":
    unittest.main(verbosity=2)                      
