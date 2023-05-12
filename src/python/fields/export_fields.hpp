#pragma once
#include "../export_pluginplay.hpp"

namespace pluginplay {

void export_module_input(py_module_reference m);
void export_module_result(py_module_reference m);

inline void export_fields(py_module_reference m) {
    export_module_input(m);
    export_module_result(m);
}

} // namespace pluginplay
