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

#include <pluginplay/module/module_class.hpp>
#include <pluginplay/module_manager/module_manager_class.hpp>
#include <sstream>

namespace pluginplay::printing {

void add_letter(std::string& code) {
    assert(code.size() == 2);
    for(int i = 2; i >= 0; --i) {
        if(code[i] == 'Z') {
            code[i] = 'A';
        } else {
            code[i]++;
            return;
        }
    }
    throw std::runtime_error("String is maxed out");
}

void print_submods(const std::string module_key,
                   const pluginplay::ModuleManager& mm, std::stringstream& ss,
                   std::string& module_code) {
    const auto& mm_module = mm.at(module_key); // Results in a PluginPlay Module
    const auto& submods = mm_module.submods(); // Results in list of Submodules
    auto main_mod_code  = module_code;

    // Key is the ID/Tag, Value is the reference to the Module
    for(const auto& [key, value] : submods) {
        ss << "\n\t" << main_mod_code << "-->"
           << "|" << key << "| ";
        if(value.has_module() == false) {
            add_letter(module_code);
            ss << module_code << "[No Submodule associated with Key]";
        } else {
            add_letter(module_code);
            ss << module_code << "[" << value.get_name() << "]";
            print_submods(value.get_name(), mm, ss, module_code);
        }
    }
}

std::stringstream create_mermaid_graph(const pluginplay::ModuleManager& mm) {
    auto n_modules = mm.size();
    std::stringstream ss;
    std::string module_code = "AAA";
    if(n_modules == 0) {
        ss << "No modules are loaded, load some modules!";
    } else {
        for(decltype(n_modules) i = 0; i < n_modules; i++) {
            ss << "\n```mermaid";
            ss << "\nflowchart LR\n";
            auto mod = mm.keys()[i];
            ss << "\t" << module_code << "[" << mod << "]";
            print_submods(mod, mm, ss, module_code);
            ss << "\n```";
            add_letter(module_code);
        }
    }
    return ss;
}
} // namespace pluginplay::printing
