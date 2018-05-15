import SDE
import unittest

def test_mm(tester, mm, keys):
    tester.assertFalse("Not an actual key" in mm)
    for k in keys:
        tester.assertTrue(k in mm)

class TestModuleManager(unittest.TestCase):
    def test_default_ctor(self):
        mm = SDE.ModuleManager()
        test_mm(self, mm, [])

if __name__ == '__main__':
    unittest.main(verbosity=2)
