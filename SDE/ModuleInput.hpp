#pragma once
#include "SDE/detail_/SDEAny.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <string>
#include <typeindex>
#include <functional>

namespace SDE {


class ModuleInput{
private:
    template<typename T>
    using cref_wrapper = std::reference_wrapper<const std::decay_t<T>>;

    template<typename T>
    using ref_wrapper = std::reference_wrapper<std::decay_t<T>>;

    template<typename T>
    struct is_c_string : std::false_type {};

    template<std::size_t N>
    struct is_c_string<const char(&)[N]> : std::true_type {};

    using any_type = detail_::SDEAny;
    using any_check = std::function<bool(const any_type&)>;

    template<typename T>
    using is_const_ref =
        std::conjunction<std::is_reference<T>,
                         std::is_const<std::remove_reference_t<T>>>;
public:
    using description_type = std::string;
    using rtti_type = std::type_index;
    template<typename T>
    using validity_check = std::function<bool(const T&)>;

    template<typename T>
    bool is_valid(T&& new_value) const {
        //Start by wrapping a const reference in an SDEAny so we can pass it to
        //each lambda
        auto wrapped_value =
            detail_::make_SDEAny<cref_wrapper<T>>(std::cref(new_value));

        //Now run it through all lambdas, short-circuiting if one fails
        for(auto& [k, v] : checks_) if(!v(wrapped_value)) return false;

        return true;
    }

    template<typename T>
    void change(T&& new_value) {
        if(type_ == rtti_type(typeid(std::nullptr_t)))
            throw std::runtime_error("Must set type first");
        using detail_::make_SDEAny;
        if(!is_valid(new_value))
            throw std::invalid_argument("Value has failed one or more checks.");
        any_type temp;
        if(is_cref_)
            temp = make_SDEAny<cref_wrapper<T>>(std::cref(new_value));
        else
            temp = make_SDEAny<std::decay_t<T>>(std::forward<T>(new_value));
        value_.swap(temp);
    }

    template<typename T>
    T value() {
        constexpr bool by_const_ref = is_const_ref<T>::value;
        constexpr bool by_value = std::is_same_v<std::decay_t<T>, T>;
        if constexpr(by_const_ref || by_value)
            return const_cast<const ModuleInput&>(*this).value<T>();
        //Want it presumably by reference
        return detail_::SDEAnyCast<T>(value_);
    }
    template<typename T>
    T value() const {
        if(is_cref_)
            return detail_::SDEAnyCast<cref_wrapper<T>>(value_);
        return detail_::SDEAnyCast<T>(value_);
    }

    template<typename T>
    void set_type() {
        constexpr bool is_ref = std::is_reference_v<T>;
        constexpr bool is_const = std::is_const_v<std::remove_reference_t<T>>;
        constexpr bool is_const_ref = is_ref && is_const;
        static_assert(is_const_ref || !is_ref,
                      "Inputs should be read-only references or by value");
        is_cref_ = is_const_ref;
        type_ = rtti_type(typeid(T));
        add_type_check<T>();
    }

    template<typename T>
    void add_check(validity_check<T> check, description_type desc=""){
        if(desc == "")
            desc = "Check #" + std::to_string(checks_.size());
        any_check temp;
        using cast_type = cref_wrapper<T>;
        temp = [check{std::move(check)}](const any_type& new_value){
            return check(detail_::SDEAnyCast<cast_type>(new_value));
        };
        checks_.emplace(std::move(desc), std::move(temp));
    }

    void hash(Hasher& h) { h(value_); }
    description_type desc = "";
    bool is_optional = false;
    bool is_transparent = false;

private:
    template<typename T>
    void add_type_check(){
        any_check check = [](const any_type& new_value){
            return new_value.type() == typeid(cref_wrapper<T>);
        };
        checks_["Type Check"] = std::move(check);
    }

    bool is_cref_ = false;
    rtti_type type_ = rtti_type(typeid(std::nullptr_t));

    Utilities::CaseInsensitiveMap<any_check> checks_;
    any_type value_;
};

}
