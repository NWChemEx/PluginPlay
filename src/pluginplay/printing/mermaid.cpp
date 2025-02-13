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
    char mod_letter     = 65;
    char submod_letter  = (65 + level);

    // Key is the ID/Tag, Value is the reference to the Module
    for(const auto& [key, value] : submods) {
        ss << "\n\tA--" << "|" << key << "|--";
        if(value.has_module() == false) {
            ss << submod_letter << level + 1
               << "[No Submodule associated with Key]\n";
            continue;
        } else {
            ss << submod_letter << level + 1 << "[" << value.get_name() << "]";
            print_submods(value.get_name(), mm, ss, level + 1);
        }
    }
}

std::string create_mermaid_graph(const pluginplay::ModuleManager& mm) {
    auto n_modules = mm.size();
    std::stringstream ss;
    for(decltype(n_modules) i = 0; i < n_modules; i++) {
        ss << "\nflowchart LR\n";
        auto mod    = mm.keys()[i];
        char letter = 65;
        ss << "\t" << letter << "[" << mod << "]";
        print_submods(mod, mm, ss, 0);
    }
    std::cout << ss.str();
    return "Hello World!";
}
