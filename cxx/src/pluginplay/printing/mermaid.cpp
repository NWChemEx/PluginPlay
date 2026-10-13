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
    if(code.size() == 0) return;
    for(int i = (code.size() - 1); i >= 0; --i) {
        if(code[i] == 'Z') {
            code[i] = 'A';
        } else {
            code[i]++;
            return;
        }
    }
}

namespace {

// Recurses over the submodules of mod. Submodules are followed through the
// Module objects, not looked up by key, because a submodule's key may be used
// by more than one plugin.
void print_submods_(const Module& mod, std::stringstream& ss,
                    std::string& module_code) {
    auto main_mod_code = module_code;

    // Key is the ID/Tag, Value is the reference to the Module
    for(const auto& [key, value] : mod.submods()) {
        ss << "\n\t" << main_mod_code << "-->"
           << "|" << key << "| ";
        if(value.has_module() == false) {
            add_letter(module_code);
            ss << module_code << "[No Submodule associated with Key]";
        } else {
            add_letter(module_code);
            ss << module_code << "[" << value.get_name() << "]";
            print_submods_(value.value(), ss, module_code);
        }
    }
}

} // namespace

void print_submods(const QualifiedKey& module_key,
                   const pluginplay::ModuleManager& mm, std::stringstream& ss,
                   std::string& module_code) {
    print_submods_(mm.at(module_key), ss, module_code);
}

std::stringstream create_mermaid_graph(const pluginplay::ModuleManager& mm) {
    std::stringstream ss;
    std::string module_code = "AAA";
    const auto keys         = mm.keys();
    if(keys.empty()) {
        ss << "No modules are loaded, load some modules!";
    } else {
        for(const auto& mod : keys) {
            // The ModuleManager's own modules are labeled by key alone
            const auto label =
              mod.plugin().empty() ? mod.key() : mod.to_string();
            ss << "\n```mermaid";
            ss << "\nflowchart LR\n";
            ss << "\t" << module_code << "[" << label << "]";
            print_submods(mod, mm, ss, module_code);
            ss << "\n```";
            add_letter(module_code);
        }
    }
    return ss;
}
} // namespace pluginplay::printing
