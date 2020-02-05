from SDE import *
import unittest

dir = os.path.dirname(os.path.realpath(__file__))
dir = os.path.dirname(dir)
cppyy.add_include_path(dir)
cppyy.include("examples/property_type.hpp")
cppyy.include("examples/module_base.hpp")
cppyy.include("examples/workflow.hpp")


class TestAreaPropertyType(unittest.TestCase):
    def setUp(self):
        self.pt = cppyy.gbl.Area()

    def test_inputs(self):
        inputs = self.pt.inputs()
        self.assertEqual(inputs.size(), 2)
        self.assertEqual(inputs["Dimension 1"].description(), "The length of the 1st dimension")
        self.assertEqual(inputs["Dimension 2"].description(), "The length of the 2nd dimension")

    def test_wrap_inputs(self):
        inputs = self.pt.wrap_inputs(self.pt.inputs(), 1.23, 4.56)
        self.assertEqual(inputs.size(), 2)
        self.assertEqual(inputs["Dimension 1"].value(), 1.23)
        self.assertEqual(inputs["Dimension 2"].value(), 4.56)
        [dim1, dim2] = cppyy.gbl.Area.unwrap_inputs(inputs)
        self.assertEqual(dim1, 1.23)
        self.assertEqual(dim2, 4.56)

    def test_results(self):
        results = self.pt.results()
        self.assertEqual(results.size(), 1)
        self.assertEqual(results["Area"].description(), "The area of the shape")

if __name__ == "__main__":
    unittest.main(verbosity=2)
