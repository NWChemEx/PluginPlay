import cppyy
cppyy.include("TestWorkflow.hpp")
cppyy.load_library("libSDE.so")
from cppyy.gbl import SDE

mm = SDE.ModuleManager()
SDE.load_modules(mm)
