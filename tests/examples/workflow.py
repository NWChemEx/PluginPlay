from sde import *
import unittest

dir = os.path.dirname(os.path.realpath(__file__))
dir = os.path.dirname(dir)
cppyy.add_include_path(dir)
cppyy.include("property_type.hpp")
cppyy.include("module_base.hpp")
cppyy.include("workflow.hpp")


class TestWorkflow(unittest.TestCase):
    def setUp(self):
        self.mm = sde.ModuleManager()
        cppyy.gbl.load_modules(self.mm)

    def test_count(self):
        self.assertEqual(self.mm.count("Rectangle"), 1)
        self.assertEqual(self.mm.count("Prism"), 1)

    def test_workflow(self):
        self.mm.change_input("Rectangle", "Name", "MyRectangle")
        dims = cppyy.gbl.vector["double"]()
        dims += [1.23, 4.56, 7.89]
        area, volume = self.mm.run_as["PrismVolume", "std::vector<double>"]("Prism", dims)
        self.assertEqual(area, 1.23*4.56)            
        self.assertEqual(volume, 1.23*4.56*7.89)


if __name__ == "__main__":
    unittest.main(verbosity=2)
