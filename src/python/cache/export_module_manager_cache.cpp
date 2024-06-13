#include "export_cache.hpp"
#include <pluginplay/cache/module_manager_cache.hpp>
namespace pluginplay {

void export_module_manager_cache(py_module_reference m) {
    py_class_type<cache::ModuleManagerCache,
                  std::shared_ptr<cache::ModuleManagerCache>>(
      m, "ModuleManagerCache")
      .def(pybind11::init<>());
}

} // namespace pluginplay
