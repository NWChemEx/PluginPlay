#pragma once
#include "../export_pluginplay.hpp"

namespace pluginplay {

void export_module_manager_class(py_module_reference m);

inline void export_module_manager(py_module_reference m) {
    export_module_manager_class(m);
}

} // namespace pluginplay
