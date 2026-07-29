# Copyright 2026 NWChemEx-Project
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

import unittest

import pluginplay as pp

class PydanticAPI(pp.PythonOnlyPropertyType):
    """A Python-only property type with one input, "input schema", and one
    result, "result schema", each an opaque Python object. A real module would
    typically constrain these to specific pydantic BaseModel subclasses; any
    Python object works for the mechanism itself, which is exercised here with
    plain dicts to avoid a pydantic dependency in this test.
    """

    def __init__(self):
        pp.PythonOnlyPropertyType.__init__(self, "PydanticAPI")
        self.declare_input("input schema").set_description(
            "A pydantic BaseModel instance"
        )
        self.declare_result("result schema").set_description(
            "A pydantic BaseModel instance"
        )


class EchoModule(pp.ModuleBase):
    def __init__(self):
        pp.ModuleBase.__init__(self)
        self.satisfies_property_type(PydanticAPI())

    def run_(self, inputs, submods):
        pt = PydanticAPI()
        (input_value,) = pt.unwrap_inputs(inputs)
        rv = self.results()
        return pt.wrap_results(rv, {"echo": input_value})


class TestPythonOnlyPropertyType(unittest.TestCase):
    def test_name(self):
        self.assertEqual(self.pt.name(), "PydanticAPI")

    def test_declare_input_and_result(self):
        self.assertEqual(list(self.pt.inputs().keys()), ["input schema"])
        self.assertEqual(list(self.pt.results().keys()), ["result schema"])
        self.assertEqual(self.pt.input_order(), ["input schema"])
        self.assertEqual(self.pt.result_order(), ["result schema"])

    def test_wrap_and_unwrap_inputs(self):
        wrapped = self.pt.wrap_inputs(self.pt.inputs(), {"x": 1})
        (unwrapped,) = self.pt.unwrap_inputs(wrapped)
        self.assertEqual(unwrapped, {"x": 1})

    def test_wrap_and_unwrap_results(self):
        wrapped = self.pt.wrap_results(self.pt.results(), {"y": 2})
        (unwrapped,) = self.pt.unwrap_results(wrapped)
        self.assertEqual(unwrapped, {"y": 2})

    def test_module_satisfies_and_runs(self):
        mod = EchoModule()
        self.assertIn("PydanticAPI", mod.python_property_types())

        # Round-trips a Python object through the module, exercising the
        # ordinary Module::run() pipeline (via run_as) -- not a bypass path.
        result = mod.run_as(self.pt, {"x": 1})
        self.assertEqual(result, {"echo": {"x": 1}})

    def test_run_as_throws_if_not_satisfied(self):
        class NotPydanticModule(pp.ModuleBase):
            def __init__(self):
                pp.ModuleBase.__init__(self)

            def run_(self, inputs, submods):
                return self.results()

        mod = NotPydanticModule()
        with self.assertRaises(RuntimeError):
            mod.run_as(self.pt, {"x": 1})

    def setUp(self):
        self.pt = PydanticAPI()
