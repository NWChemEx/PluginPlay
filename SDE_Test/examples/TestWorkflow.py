import ctypes
stdcpp = ctypes.CDLL("/home/ryan/psi4conda/lib/libstdc++.so", ctypes.RTLD_GLOBAL)

import cppyy
cppyy.include("TestWorkflow.hpp")
cppyy.load_library("libSDE.so")
from cppyy.gbl import SDE

mm = SDE.ModuleManager()
SDE.load_modules(mm)
