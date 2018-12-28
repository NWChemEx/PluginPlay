#pragma once
#include <string>
#include <typeindex>

namespace SDE {

struct SubmoduleRequest{
    using description_type = std::string;
    using rtti_type = std::type_index;

    description_type desc;
    rtti_type type;
};

} //End namespace
