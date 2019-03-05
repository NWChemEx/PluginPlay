# This tutorial shows you how to write an SDE module from Python
#
# We start by importing the SDE Python package.
from SDE import SDE

#Now you need to import the
import cppyy
import os
my_dir = os.path.dirname(os.path.realpath(__file__))
cppyy.add_include_path(os.path.dirname(my_dir))
cppyy.include("examples/TestPropertyType.hpp")
from cppyy.gbl import Area, Perimeter, PrismVolume

class Rectangle(SDE.ModuleBase):
   def __init__(self) :
       SDE.ModuleBase(self, self)
       self.satisfies_property_type[Area]()
       self.satisfies_property_type[Perimeter]()

       self.description("Computes the area of a rectangle")
       self.citation("Euclid. The Elements. 300 BCE")

       self.add_input[str]("Name")\
           .set_description("The name of the rectangle")\
           .set_default("")
       self.add_result[str]("Summary")\
           .set_description("A text summary of the results")

       self.change_input("Dimension 1")\
           .set_description("The height of the rectangle")
       self.change_input("Dimension 2")\
           .set_description("The width of the rectangle")

   def run_(self, inputs, submods):
       base, height = SDE.Area.unwrap_inputs(inputs)

       base1, height1 = SDE.Perimeter.unwrap_inputs(inputs)

       name = inputs.at("Name").value[str]()

       area = base * height
       perimeter = 2 * (base + height)

       summary = "{} has an area of {} and a perimeter of {}"\
           .format(name, area, perimeter)

       result = SDE.type.result_map()
       result = SDE.Area.wrap_inputs(result, area)
       result = SDE.Perimeter.wrap_inputs(result, area)

       return result

r = Rectangle()
print(r.inputs())
