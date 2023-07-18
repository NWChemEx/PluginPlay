#pragma once
#include "../export_pluginplay.hpp"

namespace pluginplay {

void export_document_modules(py_module_reference m);

inline void export_printing(py_module_reference m) {
    export_document_modules(m);
}

} // namespace pluginplay