#include "any/test_any.hpp"
#include "fields/test_fields.hpp"
#include "python/test_python.hpp"
#include <pybind11/pybind11.h>

namespace test_pluginplay {

PYBIND11_MODULE(py_test_pluginplay, m) {
    test_any(m);
    test_fields(m);
    test_python_wrapper(m);
}

} // namespace test_pluginplay
