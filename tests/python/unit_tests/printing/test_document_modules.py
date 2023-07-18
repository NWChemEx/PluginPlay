import unittest
import pluginplay as pp
import os
import tempfile


class TestDocumentModules(unittest.TestCase):

    def test_export_printing(self):
    
        # Get the current directory
        current_dir_path = os.getcwd()

        # Get the docs directory
        docs_dir_path = os.path.abspath(os.path.join(current_dir_path, "../docs"))

        print("Docs Directory : ", docs_dir_path)
        
        self.assertEqual('foo'.upper(), 'fOO')

    
    
    
    
    
    
    
    
    