import unittest
import os
import sys
import SDE

lib_dir = os.path.join(os.path.dirname(os.path.abspath(os.path.curdir)), "lib")
sys.path.append(lib_dir)

import DummyParameters

options = DummyParameters.params
print(options.at("The number 3"))
# add test for bad change (wrong type)
print(options.at("The number 3"))
print(options.get_description("The number 3"))
print(options.get_traits("The number 3"))
options.track_changes()
options.change("The number 3", 2)
print(options.get_traits("The number 3"))
#options.change("The number 3", -1)

print(options.at("Pi"))
options.change("Pi", 1.57)
print(options.at("Pi"))

print(options.at("A vector"))
options.change("A vector", [4, 5, 6])
print(options.at("A vector"))

print(options.at("Hello"))
options.change("Hello", "'Sup World")
print(options.at("Hello"))

if __name__ == '__main__':
    unittest.main(verbosity=2)