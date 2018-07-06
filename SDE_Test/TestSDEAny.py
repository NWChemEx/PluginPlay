import SDE.SDE as SDE
from py_sde_utils import *
import unittest

class TestSDEAny(unittest.TestCase):
    def setUp(self):
        self.corr = [1, 2, 3]
        self.da_any = make_any()
        self.da_any2 = make_any(self.corr)
    def test_cxx_wrapped(self):
        self.assertEqual(self.da_any.get(), self.corr)

    def test_py_wrapped(self):
        self.assertEqual(self.da_any2.get(), self.corr)

if __name__ == '__main__':
    unittest.main(verbosity=2)


