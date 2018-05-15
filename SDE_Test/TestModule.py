import SDE
import unittest

class ModuleType(SDE.ModuleBaseImpl):
    def __init__(self):
        SDE.ModuleBaseImpl.__init__(self)

    def run_(self, *args):
        if len(args)==1:
            return args[0]

class TestModule(unittest.TestCase):
    def test_ModuleBase(self):
        mod = SDE.ModuleBase()
        self.assertIsNotNone(mod)
    def test_ModuleType(self):
        mod = ModuleType()
        self.assertEqual(mod(1), 1)

if __name__ == '__main__':
    unittest.main(verbosity=2)
