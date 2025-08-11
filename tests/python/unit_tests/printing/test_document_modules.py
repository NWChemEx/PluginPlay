# Copyright 2023 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import shutil
import unittest

import pluginplay as pp


class TestDocumentModules(unittest.TestCase):
    def test_export_printing(self):
        # Get the current directory
        current_dir_path = os.getcwd()

        # Get the docs directory
        docs_dir_path = os.path.abspath(
            os.path.join(current_dir_path, "../docs/test_document_modules")
        )

        # Empty docs directory
        try:
            shutil.rmtree(docs_dir_path)
        except OSError as e:
            print(f"Expected Error Raised: {e}")

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

        # Remove test directory when finished
        try:
            shutil.rmtree(docs_dir_path)
        except OSError as e:
            print(f"Error: {e}")
