#pragma once
#include "SDE/detail_/SDEAny.hpp"
#include <string>
#include <memory>
#include <typeindex>

namespace SDE {

class ModuleOutput {
public:
    using description_type = std::string;
    using rtti_type = std::type_index;
    using any_type = detail_::SDEAny;
    using shared_any = std::shared_ptr<const any_type>;

    template<typename T>
    auto value() const {
        using clean_T = std::decay_t<T>;
        if constexpr(std::is_same_v<shared_any, clean_T>)
            return value_;
        else
            return detail_::SDEAnyCast<T>(*value_);
    }

    template<typename T>
    void change(T &&new_value) {
        if(type_ == rtti_type(typeid(std::nullptr_t)))
            throw std::runtime_error("Call set_type with a valid type first.");
        using clean_T = std::decay_t<T>;
        constexpr bool is_shared_ptr =
            std::is_same_v<clean_T, shared_any> ||
            std::is_same_v<clean_T, std::shared_ptr<any_type>>;
        if constexpr(is_shared_ptr)
            value_ = new_value;
        else {
            if(rtti_type(typeid(clean_T)) != type_)
                throw std::invalid_argument("New value is incorrect type");
            auto temp =
                detail_::make_SDEAny<clean_T>(std::forward<T>(new_value));
            value_ = std::make_shared<any_type>(std::move(temp));
        }

    }

    template<typename T>
    void set_type(){
        constexpr bool is_clean = std::is_same_v<std::decay_t<T>, T>;
        static_assert(is_clean, "Outputs must be unqualified types.");
        type_ = std::type_index(typeid(T));
    }

    description_type desc;
private:
    rtti_type type_ = rtti_type(typeid(std::nullptr_t));

    shared_any value_;
};

class ModuleOutputBuilder{
public:
    ModuleOutputBuilder() = delete;
    ModuleOutputBuilder(ModuleOutput& mo) : output_(&mo) {}
    ModuleOutputBuilder(const ModuleOutputBuilder& rhs) = default;
    ModuleOutputBuilder& operator=(const ModuleOutputBuilder& rhs) = delete;
    ModuleOutputBuilder(ModuleOutputBuilder&& rhs) = default;
    ModuleOutputBuilder& operator=(ModuleOutputBuilder&&) = delete;
    ~ModuleOutputBuilder() = default;

    ModuleOutputBuilder& description(std::string desc) {
        output_->desc = std::move(desc);
        return *this;
    }

    template<typename T>
    ModuleOutputBuilder& type() {
        output_->set_type<T>();
        return *this;
    }

private:
    ModuleOutput* output_ = nullptr;
};

} //End namespace
