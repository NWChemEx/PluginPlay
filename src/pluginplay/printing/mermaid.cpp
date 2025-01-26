/*
 * Copyright 2024 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "module/module_class.hpp"
#include "module_manager/module_manager_class.hpp"
#include <pluginplay/pluginplay.hpp>
#include <sstream>

void print_submods(const std::string module_key,
                   const pluginplay::ModuleManager& mm, std::stringstream& ss,
                   int level) {
    const auto& mm_module = mm.at(module_key); // Results in a PluginPlay Module
    const auto& submods = mm_module.submods(); // Results in list of Submodules
    std::string indent((level + 1) * 4, '-');

    // Key is the ID/Tag, Value is the reference to the Module
    for(const auto& [key, value] : submods) {
        ss << indent << "Step 2: Submod ID: " << key << std::endl;
        if(value.has_module() == false) {
            ss << indent
               << "Step 3: Submod Name: No Submodule associated with Key yet\n"
               << std::endl;
            continue;
        } else {
            ss << indent << "Step 3: Submod Name: " << value.get_name() << "\n"
               << std::endl;
        }
    }
}

std::string create_mermaid_graph(const pluginplay::ModuleManager& mm) {
    auto n_modules = mm.size();
    std::stringstream ss;
    for(decltype(n_modules) i = 0; i < n_modules; i++) {
        auto mod = mm.keys()[i];
        ss << "Step 1: " << mod << std::endl;
        std::string indent(i * 4, '-');
        ss << indent;
        print_submods(mod, mm, ss, i);
    }
    std::cout << ss.str();
    return "Hello World!";
}
