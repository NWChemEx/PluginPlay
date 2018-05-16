import unittest
import SDE

class PyProperty2(SDE.TestProperty2API):
    def run(self, i):
        return i + 1

# This fixture tests the member functions of ModuleBase
class TestModuleBase(unittest.TestCase):
    def test_property2(self):
        mod = PyProperty2()
        self.assertEqual(mod.run(1), 2)

if __name__ == '__main__':
    unittest.main(verbosity=2)
