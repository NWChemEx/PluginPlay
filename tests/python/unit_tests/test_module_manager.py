import pluginplay as pp
import py_test_pluginplay as test_pp
import unittest

class TestModuleManager(unittest.TestCase):
    def test_count(self):
        # Defaulted is always false
        self.assertFalse(self.defaulted.count('not a key'))

        # False if we don't have a mod
        self.assertFalse(self.has_mods.count('not a key'))

        # True if we do
        self.assertTrue(self.has_mods.count('C++ no PT'))


    def test_size(self):
        # Defaulted MMs are empty
        self.assertEqual(self.defaulted.size(), 0)
        self.assertEqual(self.has_mods.size(), self.corr_total)


    def test_at(self):
        # Throws if there's no implementation
        self.assertRaises(Exception, self.defaulted.at, 'not a key')

        # Throws if the key is invalid
        self.assertRaises(Exception, self.has_mods.at, 'not a key')

        # Can actually get a module
        self.assertNotEqual(self.has_mods.at('C++ no PT'), None)


    def test_copy_module(self):
        # Throws if there's no implementation
        self.assertRaises(Exception, self.defaulted.copy_module, 'foo', 'bar')

        # Throws if the module to copy does not exist
        has_mods = self.has_mods
        self.assertRaises(Exception, has_mods.copy_module, 'not a key', 'foo')

        # Throws if the to key is in use
        from_key = 'C++ no PT'
        to_key = 'C++ Null PT'
        self.assertRaises(Exception, has_mods.copy_module, from_key, to_key)

        # Can actually copy module
        has_mods.copy_module(from_key, 'foo')
        self.assertEqual(has_mods.size(), self.corr_total + 1)
        self.assertTrue(has_mods.count('foo'))


    def test_erase(self):
        # Is a no-op if defaulted
        bad_key = 'not a key'
        self.defaulted.erase(bad_key)
        self.assertEqual(self.defaulted.count(bad_key), 0)

        # No-op if module doesn't exist
        self.has_mods.erase(bad_key)
        self.assertEqual(self.defaulted.count(bad_key), 0)

        # Can actually erase
        self.has_mods.erase('C++ no PT')
        self.assertEqual(self.has_mods.size(), self.corr_total - 1)
        self.assertFalse(self.has_mods.count('C++ no PT'))


    def test_rename_module(self):
        # Throws if there's not implementation
        self.assertRaises(Exception, self.defaulted.rename_module, 'foo', 'bar')

        # Throws if from is not a valid key
        has_mods = self.has_mods
        self.assertRaises(Exception, has_mods.rename_module, 'not a key', 'bar')

        # Throws if to key is in use
        from_key = 'C++ no PT'
        to_key = 'C++ Null PT'
        self.assertRaises(Exception, has_mods.rename_module, from_key, to_key)

        # Can actually rename
        has_mods.rename_module(from_key, 'key not in use')
        self.assertEqual(has_mods.size(), self.corr_total)
        self.assertFalse(has_mods.count(from_key))
        self.assertTrue(has_mods.count('key not in use'))


    def test_change_input(self):
        # Throws if there's no modules
        self.assertRaises(Exception, self.defaulted.change_input, 'a', 'b', 1)

        # Throws if module does not exist
        has_mods_fxn = self.has_mods.change_input
        opt_key = 'Option 1'
        bad_key = 'not a key'
        self.assertRaises(Exception, has_mods_fxn, bad_key, opt_key, 1)

        # Throws if option does not exist
        mod_key = 'C++ module which isn\'t ready'
        self.assertRaises(Exception, has_mods_fxn, mod_key, bad_key, 1)

        # Throws if a bad value
        self.assertRaises(Exception, has_mods_fxn, mod_key, opt_key, [42])

        # Can actually change the value
        has_mods_fxn(mod_key, opt_key, 1)
        opt = self.has_mods.at(mod_key).inputs()[opt_key].value()
        self.assertEqual(opt, 1)


    def test_change_submod(self):
        # Throws if there's no modules
        bad_key = 'not a key'
        fxn2test = self.defaulted.change_submod
        self.assertRaises(Exception, fxn2test, not_a_key, not_a_key, not_a_key)

        #Throws if the module's key is invalid
        mod_key = 'C++ module needing a submodule'
        submod_key = 'Submodule 1'
        to_key = 'C++ Null PT'
        fxn2test = self.has_mods.change_submod
        self.assertRaises(Exception, fxn2test, bad_key, submod_key, to_key)

        # Throws if the submodule key does not exist
        self.assertRaises(Exception, fxn2test, mod_key, bad_key, to_key)

        # Throws if the to-key is bad
        self.assertRaises(Exception, fxn2test, mod_key, submod_key, bad_key)

        # Can actually change the submod
        fxn2test(mod_key, submod_key, to_key)
        new_submod = self.has_mods.at(mod_key).submods()[submod_key]
        self.assertEqual(new_submod, )


    def test_run_as(self):
        pass


    def setUp(self):
        self.defaulted = pp.defaulted_mm()
        self.has_mods  = test_pp.get_mm()
        self.corr_total = 10  # This should be the correct number of modules
