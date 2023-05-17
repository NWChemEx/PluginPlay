import pluginplay as pp
import py_test_pluginplay as test_pp
import unittest

class TestModule(unittest.TestCase):
    def test_unlocked_copy(self):
        other_defaulted = self.defaulted.unlocked_copy()
        self.assertEqual(self.defaulted, other_defaulted)

        self.ready_mod.lock()
        other_ready = self.ready_mod.unlocked_copy()
        self.assertFalse(other_ready.locked())
        self.assertNotEqual(other_ready, self.ready_mod)


    def test_has_module(self):
        self.assertFalse(self.defaulted.has_module())
        self.assertTrue(self.has_desc.has_module())
        self.assertTrue(self.has_cite.has_module())
        self.assertTrue(self.not_ready.has_module())
        self.assertTrue(self.need_submod.has_module())
        self.assertTrue(self.ready_mod.has_module())


    def test_has_description(self):
        # See issue #301
        #self.assertFalse(self.defaulted.has_description())

        self.assertTrue(self.has_desc.has_description())
        self.assertFalse(self.has_cite.has_description())
        self.assertFalse(self.not_ready.has_description())
        self.assertFalse(self.need_submod.has_description())
        self.assertFalse(self.ready_mod.has_description())


    def test_locked(self):
        self.assertFalse(self.defaulted.locked())
        self.assertFalse(self.has_desc.locked())
        self.assertFalse(self.has_cite.locked())
        self.assertFalse(self.not_ready.locked())
        self.assertFalse(self.need_submod.locked())
        self.assertFalse(self.ready_mod.locked())


    def test_list_not_ready(self):
        # Throws if there's no implementation b/c default constructed
        self.assertRaises(Exception, self.defaulted.list_not_ready)
        self.assertEqual(self.has_desc.list_not_ready(), {})
        self.assertEqual(self.has_cite.list_not_ready(), {})

        # Has two inputs that aren't set
        corr = {'Inputs' : {'Option 2', 'Option 1'}}
        self.assertEqual(self.not_ready.list_not_ready(), corr)

        # If one input comes from say a property type
        corr = {'Inputs' : {'Option 2'}}
        inps = test_pp.OneIn().inputs()
        self.assertEqual(self.not_ready.list_not_ready(inps), corr)

        # Not ready b/c of an unset submodule
        corr = {'Submodules' : {'Submodule 1', }}
        self.assertEqual(self.need_submod.list_not_ready(), corr)

        # Is actually ready to run
        self.assertEqual(self.ready_mod.list_not_ready(), {})


    def test_reset_cache(self):
        # The main cache lives in the PIMPL, not the implementation (so this
        # should work)
        self.defaulted.reset_cache()

        # For now just testing if we can call the function
        self.has_desc.reset_cache()


    def test_reset_internal_cache(self):
        # Throws if doesn't have cache b/c default constructed
        self.assertRaises(Exception, self.defaulted.reset_internal_cache)

        # For now just testing if we can call the function
        self.has_desc.reset_internal_cache()


    def test_is_memoizable(self):
        # Throws if there's no implementation b/c default constructed
        self.assertRaises(Exception, self.defaulted.is_memoizable)

        self.assertTrue(self.has_desc.is_memoizable())
        self.assertTrue(self.has_cite.is_memoizable())
        self.assertTrue(self.not_ready.is_memoizable())

        # Throws if submodule is not set yet
        self.assertRaises(Exception, self.need_submod.is_memoizable)

        self.assertTrue(self.ready_mod.is_memoizable())


    def test_turn_off_memoization(self):
        self.assertTrue(self.has_desc.is_memoizable())
        self.has_desc.turn_off_memoization()
        self.assertFalse(self.has_desc.is_memoizable())


    def test_turn_on_memoization(self):
        self.has_desc.turn_off_memoization()
        self.assertFalse(self.has_desc.is_memoizable())
        self.has_desc.turn_on_memoization()
        self.assertTrue(self.has_desc.is_memoizable())


    def test_lock(self):
        # Can lock a defaulted instance
        self.defaulted.lock()


    def test_results(self):
        self.assertRaises(Exception, self.defaulted.results)


    def test_inputs(self):
        self.assertRaises(Exception, self.defaulted.inputs)


    def test_submods(self):
        self.assertRaises(Exception, self.defaulted.submods)


    def test_property_types(self):
        self.assertRaises(Exception, self.defaulted.property_types)


    def test_description(self):
        # Throws if no description is set
        self.assertRaises(Exception, self.defaulted.description)


    def test_citations(self):
        # Throws if no citations are set
        self.assertRaises(Exception, self.defaulted.citations)

        self.assertEqual(self.has_cite.citations(), ['A citation'])


    def test_change_input(self):
        self.assertRaises(Exception, self.defaulted.change_input, 'hello')


    def test_change_submod(self):
        self.assertRaises(Exception, self.defaulted.change_submod, 'hello')


    def test_run_as(self):
        self.assertRaises(Exception, self.defaulted.run_as, test_pp.NullPT())


    def test_run(self):
        self.assertRaises(Exception, self.defaulted.run, {})


    def test_profile_info(self):
        self.assertEqual(self.defaulted.profile_info(), '')


    def test_submod_uuids(self):
        # Throws if no module is set
        self.assertRaises(Exception, self.defaulted.submod_uuids)


    def test_uuid(self):
        # Throws if no module is set
        self.assertRaises(Exception, self.defaulted.uuid)


    def test_comparisons(self):
        # Default vs default
        other_default = pp.Module()
        self.assertEqual(self.defaulted, other_default)
        self.assertFalse(self.defaulted != other_default)


    def setUp(self):
        mm = test_pp.get_mm()
        self.defaulted = pp.Module()
        self.has_desc    = mm.at('C++ with description')
        self.has_cite    = mm.at('C++ with a citation')
        self.not_ready   = mm.at('C++ module which also isn\'t ready')
        self.need_submod = mm.at('C++ module needing a submodule')
        self.ready_mod   = mm.at('C++ module which is ready to run')


class TestPrintNotReady(unittest.TestCase):
    def test_print_not_ready(self):
        self.assertRaises(Exception, pp.print_not_ready, pp.Module())
