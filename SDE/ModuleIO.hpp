#pragma once
#include "SDE/detail_/SDEAny.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <functional>


namespace SDE {

class ModuleIO {
public:
    using description_type = std::string;

    virtual ~ModuleIO() = default;

    template<typename T>
    using value_checker = std::function<bool(const T&)>;

    const auto& type() const noexcept { return value_.type(); }

    template<typename T>
    const T& value() const{ return detail_::SDEAnyCast<const T&>(value_); }

    template<typename T>
    void change(T&& new_value) {
        auto temp = wrap_value(std::forward<T>(new_value));
        if(!is_valid(temp))
            throw std::invalid_argument("Not a valid option value");
        value_.swap(temp);
    }

    template<typename T>
    bool is_valid(T&& test_value) {
        return is_valid_(wrap_value(std::forward<T>(test_value)));
    }

    template<typename T>
    void add_check(value_checker<T> check, description_type desc=""){
        if(desc == "")
            desc = "Check #" + std::to_string(checks_.size());
        auto lambda = [=](const detail_::SDEAny& value) {
            return check(this->template value<T>());
        };
        checks_[desc] = lambda;
    }

    description_type desc = "";
private:
    using any_type = detail_::SDEAny;
    using check_fxn = value_checker<any_type>;
    using check_map = Utilities::CaseInsensitiveMap<check_fxn>;

    template<typename T>
    auto wrap_value(T&& test_value) const {
        using clean_T = std::decay_t<T>;
        return detail_::make_SDEAny<clean_T>(std::forward<T>(test_value));
    }

    bool is_valid_(const any_type& test_value) {
        for(auto [name, check] : checks_)
            if(!check(test_value)) return false;
        return true;
    }

    check_map checks_;

    detail_::SDEAny value_;
};

using ModuleOutput = ModuleIO;

class ModuleInput : public ModuleIO {
public:
    bool is_transparent = false;
    bool is_optional = false;
};


}; //End namespace
