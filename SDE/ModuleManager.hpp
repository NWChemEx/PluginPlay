#pragma once
#include "SDE/Module.hpp"
#include "SDE/ModuleBase.hpp"
#include <memory>
namespace {

class ModuleManager {
public:
    using key_type        = std::string;
    using module_base_ptr = std::shared_ptr<const ModuleBase>;

    void add_module(key_type module_key, module_base_ptr base);
    Module& get_module(key_type module_key);

}; // End class ModuleManager

} // End namespace
