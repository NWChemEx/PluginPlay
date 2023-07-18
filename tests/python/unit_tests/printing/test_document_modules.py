import unittest
import pluginplay as pp
import os
import tempfile
import shutil

class TestDocumentModules(unittest.TestCase):

    def test_export_printing(self):
    
        # Get the current directory
        current_dir_path = os.getcwd()

        # Get the docs directory
        docs_dir_path = os.path.abspath(os.path.join(current_dir_path, "../docs"))

        
        # Empty docs directory
        try:
            shutil.rmtree(docs_dir_path)
        except OSError as e:
            print(f"Error: {e}")
        
        
        # Create docs directory
        try:
            os.makedirs(docs_dir_path)
        except OSError as e:
            print(f"Error: {e}")
        
        
        # Check if docs directory exists
        self.assertTrue(os.path.exists(docs_dir_path))
        
        # Check if docs is a directory
        self.assertTrue(os.path.isdir(docs_dir_path))
        
        docs_file_list = os.listdir(docs_dir_path)
        
        # Check if docs directory is empty
        self.assertTrue(len(docs_file_list) == 0)
        
        docs_file_list = []
        
        mm = pp.ModuleManager()
        
        pp.document_modules(mm, docs_dir_path)
        
        docs_file_list = os.listdir(docs_dir_path)
        
        # Check if index.rst is created
        self.assertTrue(len(docs_file_list) == 1)

    
    
    
    
    
    
    
    
    