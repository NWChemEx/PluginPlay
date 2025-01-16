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

#include <pluginplay/pluginplay.hpp>

std::string hello_world(const pluginplay::ModuleManager& mm) {
    std::cout << "RUNNING IN THE SOURCE" << std::endl;
    std::cout << "ASSIGNING mm.size() TO n_modules" << std::endl;
    auto n_modules = mm.size();
    std::cout << "RUNNING FOR LOOP 1" << std::endl;
    for(decltype(n_modules) i = 0; i < n_modules; i++) {
        std::cout << "ASSIGNING m.keys()[i] TO mod" << std::endl;
        auto mod = mm.keys()[i];
        std::cout << "ASSIGNING mm.at(mod) TO module" << std::endl;
        auto module = mm.at(mod);
        std::cout << "PRINTING mod" << std::endl;
        std::cout << mod << std::endl;
        std::cout << "ASSIGNING module.submods() TO submods" << std::endl;
        auto submods = module.submods();
        std::cout << "RUNNING FOR LOOP 2" << std::endl;
        for(const auto& [key, value] : submods) {
            std::cout << "PRINTING key IN submods" << std::endl;
            std::cout << "This is a submod identifier: " << key << std::endl;
            std::cout << "ASSIGNING mm.at(key).get_name() TO submod_name"
                      << std::endl;
            auto submod_name = mm.at(key).get_name();
            std::cout << "PRINTING submod_name" << std::endl;
            std::cout << "This is the true name: " << submod_name << std::endl;
        }
    }
    return "Hello World!";
}
