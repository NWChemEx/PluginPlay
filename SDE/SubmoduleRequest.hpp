#pragma once
#include "SDE/Module.hpp"
#include <string>
#include <typeindex>

namespace SDE {
class Module;

class SubmoduleRequest {
public:
    using description_type = std::string;
    using rtti_type        = std::type_index;

    template<typename property_type, typename... Args>
    auto run_as(Args&&... args) {
        if(rtti_type(typeid(property_type)) != type_)
            throw std::invalid_argument("Wrong property type");
        return module_->run_as<property_type>(std::forward<Args>(args)...);
    }

    const description_type description() const noexcept { return desc_; }

    void hash(bphash::Hasher& h) const { module_->hash(h); }

    void change(std::shared_ptr<Module> new_module) { module_ = new_module; }

    template<typename T>
    auto& set_type() noexcept {
        type_ = std::type_index(typeid(std::decay_t<T>));
        return *this;
    }

    auto& set_description(description_type desc) noexcept {
        desc_ = std::move(desc);
        return *this;
    }

private:
    description_type desc_;

    rtti_type type_;

    std::shared_ptr<Module> module_;
};

} // namespace SDE
