#include <iostream>
#include "/home/jacob/Projects/PluginPlay/include/pluginplay/pluginplay.hpp"
#include "/home/jacob/Projects/GhostFragment/include/ghostfragment/ghostfragment.hpp"

int main() {
    pluginplay::ModuleManager mm;
    ghostfragment::load_modules(mm);

    auto mm_keys = mm.keys();

//    std::cout << "Keys: [";
//    for (size_t i = 0; i < mm_keys.size(); ++i) {
//        std::cout << "\"" << mm_keys[i] << "\"";
//        if (i < mm_keys.size() - 1) {
//            std::cout << ", ";
//        }
//    }
//    std::cout << "]" << std::endl;
    

    auto mm_module = mm.at("Fragment Based Method");

    auto mm_submods = mm_module.submods();

    auto mm_first_submod = mm_submods[1]; // Not sure how to access

//    std::cout << mm_first_submod << std::endl;

    return 0;
}
