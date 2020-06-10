from sde import *
from tests import *
import unittest

class TestWorkflow(unittest.TestCase):
    def setUp(self):
        self.mm = sde.ModuleManager()
        cppyy.gbl.load_modules(self.mm)

    def test_count(self):
        self.assertEqual(self.mm.count("Rectangle"), 1)
        self.assertEqual(self.mm.count("Prism"), 1)

    def test_workflow(self):
        self.mm.change_input("Rectangle", "Name", "MyRectangle")
        area, volume = self.mm.run_as["PrismVolume"]("Prism", [1.23, 4.56, 7.89])
        self.assertEqual(area, 1.23*4.56)            
        self.assertEqual(volume, 1.23*4.56*7.89)


if __name__ == "__main__":
    unittest.main(verbosity=2)
