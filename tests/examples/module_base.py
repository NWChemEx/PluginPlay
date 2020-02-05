from sde import *
from cppyy.gbl import std
import unittest

dir = os.path.dirname(os.path.realpath(__file__))
dir = os.path.dirname(dir)
cppyy.add_include_path(dir)

cppyy.include("examples/writing_a_property_type.hpp")
cppyy.include("examples/writing_a_module.hpp")
cppyy.include("examples/workflow.hpp")

mm = sde.ModuleManager()
cppyy.gbl.load_modules(mm)

def get_rectangle():
    mod = mm.at("Rectangle")
    return sde.Module(mod)

def get_prism():
    mod = mm.at("Prism")
    return sde.Module(mod)

class test_module_base(unittest.TestCase):
    def setUp(self):
        self.r1 = get_rectangle()
        self.r2 = get_rectangle()
        self.p1 = get_prism()
        self.p2 = get_prism()

    def test_equality(self):
        self.assertEqual(self.r1, self.r2)
        self.assertNotEqual(self.r1, self.p1)

    def test_rectangle_inputs(self):
        inputs = self.r1.inputs()
        self.assertEqual(inputs.at("Dimension 1").description(), "The height of the rectangle")
        self.assertEqual(inputs.at("Dimension 2").description(), "The width of the rectangle")
        self.assertEqual(inputs.at("Name").description(), "The name of the rectangle")        

    def test_rectangle_results(self):
        results = self.r1.results()
        self.assertEqual(results.at("Area").description(), "The area of the shape")
        self.assertEqual(results.at("Perimeter").description(), "The perimeter of the shape")
        self.assertEqual(results.at("Summary").description(), "A text summary of the results")

    def test_rectangle_submods(self):
        self.assertEqual(self.r1.submods().size(), 0)

    def test_rectangle_run(self):
        inputs = self.r1.inputs()
        inputs.at("Dimension 1").change["double"](1.23)
        inputs.at("Dimension 2").change["double"](4.56)
        inputs.at("Name").change["std::string"]("Test")
        results = self.r1.run(inputs)
        self.assertAlmostEqual(results.at("Area").value(), 5.6088)
        self.assertAlmostEqual(results.at("Perimeter").value(), 11.58)
        self.assertEqual(results.at("Summary").value["std::string"](), "Test has an area of 5.608800 and a perimeter of 11.580000")

    def test_prism_inputs(self):
        inputs = self.p1.inputs()
        self.assertEqual(inputs.at("Dimensions").description(), "1st 2 elements are for the base, last is height")

    def test_prism_results(self):
        results = self.p1.results()
        self.assertEqual(results.at("Base Area").description(), "The area of the base")
        self.assertEqual(results.at("Volume").description(), "The volume of the prism")

    def test_prism_submods(self):
        submods = self.p1.submods()
        self.assertEqual(submods.size(), 1)
        self.assertEqual(submods.at("area").description(), "Submodule used to compute base's area")

    def test_prism_run(self):
        inputs = self.p1.inputs()
        dims = std.vector["double"]()
        dims += [1.23, 4.56, 7.89]
        inputs.at("Dimensions").change["const std::vector<double>&"](dims)
        results= self.p1.run(inputs)
        area, volume = cppyy.gbl.PrismVolume.unwrap_results(results)
        self.assertAlmostEqual(area, 5.6088)
        self.assertAlmostEqual(volume, 44.253432)


if __name__ == "__main__":
    unittest.main(verbosity=2)                      
