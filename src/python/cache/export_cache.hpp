#pragma once
#include "../export_pluginplay.hpp"

namespace pluginplay {

void export_module_manager_cache(py_module_reference m);

inline void export_cache(py_module_reference m) {
    auto m_cache = m.def_submodule("cache");
    export_module_manager_cache(m_cache);
}

} // namespace pluginplay
