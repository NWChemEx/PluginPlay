#pragma once
#include <string>
#include <typeindex>

namespace SDE {

struct SubmoduleRequest {
    using description_type = std::string;
    using rtti_type        = std::type_index;

    description_type desc;

    rtti_type property_type;

    std::shared_ptr<Module> module;

    template<typename property_type, typename... Args>
    auto run_as(Args&&... args) {
        return module->run_as<property_type>(std::forward<Args>(args)...);
    }
};

} // namespace SDE
