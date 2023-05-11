#include "any/export_any.hpp"
#include "python/export_python.hpp"
#include <pybind11/pybind11.h>

namespace pluginplay {

PYBIND11_MODULE(pluginplay, m) {
    any::export_any(m);
    python::export_python(m);
}

} // namespace pluginplay
