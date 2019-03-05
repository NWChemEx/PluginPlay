from SDE import SDE
import cppyy
import os
my_dir = os.path.dirname(os.path.realpath(__file__))
cppyy.add_include_path(os.path.dirname(my_dir))
cppyy.include("examples/TestWorkflow.hpp")
from cppyy.gbl import Area, Prism, PrismVolume, Rectangle


def load_modules(mm):
    mm.add_module("Rectangle", Rectangle())
    mm.add_module("Prism", Prism())
    mm.set_default[Area]("Rectangle")
    mm.set_default[PrismVolume]("Prism")

def example_main():
    mm = SDE.ModuleManager()
    load_modules(mm)

    mm.change_input("Rectangle", "Name", "My Rectangle")

    dims = [1.23, 4.56, 7.89]
    volume,area = mm.run_as[PrismVolume]("Prism", dims)
    print(volume, area)

example_main()
